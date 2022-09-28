#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define BUFFER_SIZE 5*1024*1024
char buffer[BUFFER_SIZE];

volatile sig_atomic_t received = 0;
volatile sig_atomic_t exiting = 0;

void handler(int sig)
{
  static int i = 0;
  i++;
  /*if(i==16)
  {
    exiting=1;
  }*/
  signal(SIGALRM,handler);
  alarm(1);
  printf("Download speed: %lf KiB/s\n",(double)received/1024);
  received=0;
}

int client(char *host, char *port)
{
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(atoi(port));
  server_address.sin_addr.s_addr = inet_addr(host);
  int sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(sock==-1)
  {
    perror("socket");
    return -1;
  }
  if(connect(sock,(struct sockaddr *)&server_address,sizeof(server_address))==-1)
  {
    close(sock);
    perror("connect");
    return -1;
  }
  signal(SIGALRM,handler);
  alarm(1);
  int err = 0;
  while(!exiting)
  {
        ssize_t e;
        if((e=recv(sock,buffer+received%BUFFER_SIZE,BUFFER_SIZE-received%BUFFER_SIZE,0))==-1)
        {
          if(errno==EINTR)
          {
            continue;
          }
          perror("recv");
          err=1;
          break;
        }
        if(e==0)
        {
          err=0;
          break;
        }
        received+=e;
  }
  shutdown(sock,SHUT_RDWR);
  close(sock);
  return err;
}

int server(char *port)
{
  signal(SIGCHLD,SIG_IGN);
  signal(SIGPIPE,SIG_IGN);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(port));
  addr.sin_addr.s_addr = INADDR_ANY;
  int sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(sock==-1)
  {
    perror("socket");
    return -1;
  }
   static int one = 1;
  if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one))==-1)
  {
    close(sock);
    perror("setsockopt");
    return -1;
  }
  if(bind(sock,(struct sockaddr *)&addr,sizeof(addr))==-1)
  {
    close(sock);
    perror("bind");
    return -1;
  }
  if(listen(sock,50)==-1)
  {
    close(sock);
    perror("listen");
    return -1;
  }
  for(;;)
  {
    struct sockaddr_in caddr;
    socklen_t caddr_length = sizeof(caddr);
    int csock = accept(sock,(struct sockaddr *)&caddr,&caddr_length);
    if(csock==-1)
    {
      perror("accept");
      continue;
    }
    pid_t pid = fork();
    if(pid==-1)
    {
      shutdown(csock,SHUT_RDWR);
      close(csock);
      perror("fork");
      continue;
    }
    if(!pid)
    {
      int flag = 1;
      int err = 0;
      close(sock);
      while(flag)
      {
        size_t sent = 0;
        while(sent<BUFFER_SIZE)
        {
          ssize_t e;
          if((e=send(csock,buffer+sent,BUFFER_SIZE-sent,0))==-1)
          {
            if(errno==EINTR)
            {
              continue;
            }
            err = 1;
            perror("send");
            flag = 0;
            break;
          }
          if(e==0)
          {
            err = 0;
            flag = 0;
            break;
          }
          sent+=e;
        }
      }
      shutdown(csock,SHUT_RDWR);
      close(csock);
      return err;
    }
    else
    {
      close(csock);
    }
  }
}

int main(int argc, char *argv[])
{
  if(argc!=3)
  {
    fprintf(stderr,"Usage: %s -l port\n       %s host port\n",argv[0],argv[0]);
    return 1;
  }
  if(!strcmp(argv[1],"-l"))
  {
    return server(argv[2]);
  }
  else
  {
    return client(argv[1],argv[2]);
  }
}
