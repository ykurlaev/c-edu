#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>
#include "filetransfer.h"

struct client
{
  enum
  {
    initial, recieving, readytosend, reading, sending, todelete
  } state;
  int socket;
  int fd;
  char *inbuffer;
  size_t recieved;
  char *outbuffer;
  size_t length;
  size_t sent;
  struct client *next;
};

struct client *add_client(struct client *head, int socket)
{
  struct client *list = head;
  struct client *node = malloc(sizeof(struct client));
  if(!node)
  {
    return NULL;
  }
  node->inbuffer = malloc(BUFFER_SIZE);
  if(!node->inbuffer)
  {
    free(node);
    return NULL;
  }
  node->outbuffer = malloc(BUFFER_SIZE);
  if(!node->outbuffer)
  {
    free(node->inbuffer);
    free(node);
    return NULL;
  }
  node->next = NULL;
  node->socket = socket;
  node->fd = 0;
  node->state = recieving;
  node->recieved = 0;
  node->sent = 0;
  node->length = 0;
  if(!list)
  {
    return node;
  }
  else
  {
    while(list->next)
    {
      list = list->next;
    }
    list->next = node;
    return head;
  }
}

void destroy_client(struct client *client)
{
  if(client->fd!=-1)
  {
    if(close(client->fd)==-1)
    {
      perror("Warning! Can't close file");
    }
  }
  if(shutdown(client->socket,SHUT_RDWR)==-1 || close(client->socket)==-1)
  {
    perror("Warning! Can't close socket");
  }
  free(client->inbuffer);
  free(client->outbuffer);
  free(client);
}

struct client *clean_clients(struct client *head)
{
  struct client *node, *prev;
  while(head&&head->state==todelete)
  {
    struct client *tmp = head;
    head = head->next;
    destroy_client(tmp);
  }
  prev = node = head;
  while(node)
  {
    if(node->state==todelete)
    {
      prev->next=node->next;
      destroy_client(node);
      node = prev->next;
      continue;
    }
    prev = node;
    node = node->next;
  }
  return head;
}

int get_filename(struct client *client)
{
  char *eol = NULL;
  ssize_t recieved;
  recieved=recv(client->socket,client->inbuffer+client->recieved,BUFFER_SIZE-client->recieved,0);
  if(recieved==-1)
  {
    perror("Warning! Can't recieve");
    return -1;
  }
  client->recieved+=recieved;
  client->inbuffer[recieved] = '\0';
  eol = strchr(client->inbuffer,'\n');
  if(!eol&&!recieved)
  {
    fprintf(stderr,"Warning! Protocol error!\n");
    client->fd = -1;
    client->state = readytosend;
  }
  if(eol)
  {
    *eol = '\0';
    client->state = readytosend;
  }
  return 0;
}

int prepare_to_send(struct client *client)
{
  char err;
  char protocol_error = 0;
  if(client->fd==-1)
  {
    protocol_error = 1;
  }
  else
  {
    client->fd = open(client->inbuffer,O_RDONLY);
  }
  if(client->fd==-1)
  {
    char *error;
    if(!protocol_error)
    {
      error = strerror(errno);
      if(!error)
      {
        error = "Strange error";
      }
    }
    else
    {
      error = "Protocol error";
    }
    client->length = strlen(error);
    char *tmp = realloc(client->outbuffer,client->length+1);
    if(!tmp)
    {
      perror("Error! Can't allocate memory");
      return -1;
    }
    client->outbuffer = tmp;
    strcpy(client->outbuffer,error);
    err = '-';
    client->state = sending;
  }
  else
  {
    err = '+';
    client->state = reading;
  }
  if(send(client->socket,&err,1,0)==-1)
  {
    perror("Warning! Can't send");
    return -1;
  }
  return 0;
}

int read_file(struct client *client)
{
  ssize_t e = read(client->fd,client->outbuffer+client->length,BUFFER_SIZE-client->length);
  if(e==-1)
  {
    perror("Can't read file");
    return -1;
  }
  client->length+=e;
  if(e==0)
  {
    if(close(client->fd)==-1)
    {
      perror("Warning! Can't close file");
    }
    client->fd = -1;
    client->state = sending;
  }
  if(client->length>=BUFFER_SIZE)
  {
    client->state = sending;
  }
  return 0;
}

int send_file(struct client *client)
{
  ssize_t e = send(client->socket,client->outbuffer+client->sent,
                   client->length-client->sent,0);
  if(e==-1)
  {
    perror("Warning! Can't send");
    return -1;
  }
  client->sent+=e;
  if(client->sent>=client->length)
  {
    if(client->fd!=-1)
    {
      client->length=0;
      client->sent=0;
      client->state = reading;
    }
    else
    {
      client->state = todelete;
    }
  }
  return 0;
}

int init_server(in_port_t port)
{
  int server_socket;
  static const int one = 1;
  struct sockaddr_in sockaddr;
  signal(SIGPIPE,SIG_IGN);
  server_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(!server_socket)
  {
    perror("Can't create socket");
    return -1;
  }
  if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one))==-1)
  {
    perror("Can't set socket options");
    close(server_socket);
    return -1;
  }
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = port;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  if(bind(server_socket,(struct sockaddr *)&sockaddr,sizeof(struct sockaddr_in))==-1)
  {
    perror("Cant't bind");
    close(server_socket);
    return -1;
  }
  if(listen(server_socket,50)==-1)
  {
    perror("Can't listen");
    close(server_socket);
    return -1;
  }
  return server_socket;
}

int run_server(in_port_t port)
{
  int server_socket = init_server(port);
  struct client *clients = NULL;
  if(server_socket==-1)
  {
    return EXIT_FAILURE;
  }
  for(;;)
  {
    struct client *client;
    fd_set rd,wr;
    int nfds = server_socket+1;
    FD_ZERO(&rd);
    FD_ZERO(&wr);
    FD_SET(server_socket,&rd);
    client = clients;
    while(client)
    {
      nfds = (client->socket>=nfds)?(client->socket+1):(nfds);
      nfds = (client->fd>=nfds)?(client->fd+1):(nfds);
      if(client->state==recieving)
      {
        FD_SET(client->socket,&rd);
      }
      else if(client->state==readytosend || client->state==sending)
      {
        FD_SET(client->socket,&wr);
      }
      else if(client->state==reading)
      {
        FD_SET(client->fd,&rd);
      }
      client = client->next;
    }
    select(nfds,&rd,&wr,NULL,NULL);
    if(FD_ISSET(server_socket,&rd))
    {
      struct sockaddr_in client_address;
      socklen_t client_address_length = sizeof(client_address);
      int client_socket = accept(server_socket,(struct sockaddr *)&client_address,&client_address_length);
      if(client_socket==-1)
      {
        perror("Can't accept client");
      }
      else
      {
        clients = add_client(clients,client_socket);
      }
    }
    client = clients;
    while(client)
    {
      if(client->state==reading && FD_ISSET(client->fd,&rd))
      {
        if(read_file(client)==-1)
        {
          client->state=todelete;
        }
      }
      if(client->state==recieving && FD_ISSET(client->socket,&rd))
      {
        if(get_filename(client)==-1)
        {
          client->state=todelete;
        }
      }
      if((client->state==readytosend || client->state==sending) && FD_ISSET(client->socket,&wr))
      {
        if(client->state==readytosend)
        {
          if(prepare_to_send(client)==-1)
          {
            client->state=todelete;
          }
        }
        else if(client->state==sending)
        {
          if(send_file(client)==-1)
          {
            client->state=todelete;
          }
        }
      }
      client = client->next;
    }
    clients = clean_clients(clients);
  }
}
