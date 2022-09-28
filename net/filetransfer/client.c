#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "filetransfer.h"

int run_client(in_addr_t address, in_port_t port)
{
  int client_socket;
  char outbuffer[BUFFER_SIZE+1];
  char filename[BUFFER_SIZE+1];
  char inbuffer[BUFFER_SIZE];
  char *eol;
  char *buf;
  FILE *file;
  ssize_t sent;
  ssize_t recieved;
  char *tfilename;
  char err;
  struct sockaddr_in sockaddr;
  fgets(outbuffer,sizeof(outbuffer),stdin);
  eol = strchr(outbuffer,'\n');
  if(eol==outbuffer)
  {
    return 1;
  }
  if(!eol)
  {
    fprintf(stderr,"Warning, filename is too long.\n");
    eol=outbuffer+BUFFER_SIZE;
  }
  client_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(client_socket==-1)
  {
    perror("Can't create socket");
    return 1;
  }
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = port;
  sockaddr.sin_addr.s_addr = address;
  if(connect(client_socket,(struct sockaddr *)&sockaddr,sizeof(sockaddr))==-1)
  {
    perror("Warning, can't connect to remote host");
    shutdown(client_socket,SHUT_RDWR);
    close(client_socket);
    return 1;
  }
  buf = outbuffer;
  while((sent=send(client_socket,buf,eol-buf+2,0))<eol-buf+2)
  {
    if(sent==-1)
    {
      perror("Warning, error sending");
      shutdown(client_socket,SHUT_RDWR);
      close(client_socket);
      return 1;
    }
    buf+=sent;
  }
  tfilename = strrchr(outbuffer,'/');
  if(!tfilename)
  {
    tfilename = outbuffer;
  }
  strncpy(filename,tfilename+1,eol-tfilename);
  filename[eol-tfilename-1] = '\0';
  if(recv(client_socket,&err,1,0)!=1)
  {
    perror("recv");
    return 1;
  }
  if(err=='+')
  {
    int fd = open(filename,O_WRONLY|O_CREAT|O_EXCL,0666);
    if(fd==-1)
    {
      perror("open");
      shutdown(client_socket,SHUT_RDWR);
      close(client_socket);
      return -1;
    }
    file = fdopen(fd,"wb");
    if(!file)
    {
      perror("fdopen");
      shutdown(client_socket,SHUT_RDWR);
      close(client_socket);
      return 1;
    }
  }
  else
  {
    file = stdout;
  }
  while((recieved=recv(client_socket,inbuffer,sizeof(inbuffer),0)))
  {
    if(recieved==-1)
    {
      perror("Warning, error recieving");
      shutdown(client_socket,SHUT_RDWR);
      close(client_socket);
      return 1;
    }
    fwrite(inbuffer,1,recieved,file);
    if(err!='+')
    {
      fprintf(file,"\n");
    }
  }
  fclose(file);
  shutdown(client_socket,SHUT_RDWR);
  close(client_socket);
  return 0;
}
