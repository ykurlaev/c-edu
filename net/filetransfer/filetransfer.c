#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "filetransfer.h"

#define DEFAULT_PORT 4242

static void usage(char *s)
{
  fprintf(stderr,"%s -l [port]\n%s address [port]\n",s,s);
}

int parse_args(int argc, char *argv[], in_addr_t *address, in_port_t *port, int *listen)
{
  *listen = 0;
  if(argc<2)
  {
    return -1;
  }
  if(argv[1][0]=='-'&&argv[1][1]=='l')
  {
    *listen = 1;
  }
  else
  {
    *address = inet_addr(argv[1]);
    if(*address==(in_addr_t)-1)
    {
      return -1;
    }
  }
  if(argc>=3)
  {
    char *rest;
    *port = htons(strtol(argv[2],&rest,10));
    if(*rest!='\0')
    {
      *port = htons(DEFAULT_PORT);
    }
  }
  else
  {
    *port = htons(DEFAULT_PORT);
  }
  return 0;
}

int main(int argc, char *argv[])
{
  int server;
  in_port_t port;
  in_addr_t address;
  if(parse_args(argc,argv,&address,&port,&server)==-1)
  {
    usage(argv[0]);
    return EXIT_FAILURE;
  }
  if(server)
  {
    return run_server(port);
  }
  else
  {
    return run_client(address,port);
  }
}
