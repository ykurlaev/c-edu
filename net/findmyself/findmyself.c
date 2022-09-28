#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define DEFAULT_PORT 4356

int counter(int port)
{
  int count=0;
  char msg[6];
  char buffer[6];
  int my_socket=socket(AF_INET,SOCK_DGRAM,0);
  if(my_socket==-1)
  {
    perror("socket");
    return -1;
  }
  int broadcast_socket=socket(AF_INET,SOCK_DGRAM,0);
  if(broadcast_socket==-1)
  {
    perror("socket");
    return -1;
  }
  int a=1;
  if((setsockopt(broadcast_socket,SOL_SOCKET,SO_BROADCAST,&a,sizeof(a)))==-1)
  {
    perror("setsockopt");
    return -1;
  }
  struct sockaddr_in addr;
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=port;
  if((bind(my_socket,(struct sockaddr *)&addr,sizeof(addr)))==-1)
  {
    perror("bind");
    return -1;
  }
  addr.sin_addr.s_addr=htonl(INADDR_BROADCAST);
  sprintf(msg,"R%5d",rand());
  if((sendto(broadcast_socket,msg,sizeof(msg),0,(struct sockaddr*)&addr,sizeof(addr)))==-1)
  {
    perror("sendto");
    return -1;
  }
  struct sockaddr_in peer_addr;
  socklen_t peer_addr_size=sizeof(peer_addr);
  int flag=1;
  while(flag)
  {
    int e;
    if((e=recvfrom(my_socket,buffer,sizeof(buffer),0,
       (struct sockaddr *)&peer_addr,&peer_addr_size))==-1)
    {
      perror("recvfrom");
    }
    buffer[e]='\0';
    msg[0]='S';
    if(buffer[0]=='S'&&!strcmp(buffer,msg)||buffer[0]=='R')
    {
      count++;
    }
    msg[0]='R';
    if(buffer[0]=='R'&&strcmp(buffer,msg)) //don't answer myself
    {
      buffer[0]='S';
      if((sendto(my_socket,buffer,e,0,
         (struct sockaddr*)&peer_addr,peer_addr_size))==-1)
      {
        perror("sendto");
      }
    }
    printf("%d: %s\n",count,inet_ntoa(peer_addr.sin_addr));
  }
  close(my_socket);
}

int main(int argc,char *argv[])
{
  srand(time(0));
  int port=htons(DEFAULT_PORT);
  if(argc>=2)
  {
    port=htons(atoi(argv[1]));
  }
  if(counter(port)==-1)
  {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
