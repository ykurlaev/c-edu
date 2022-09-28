#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
  FILE *file;
  if(argc<2)
  {
    return EXIT_FAILURE;
  }

  printf("uid: %d, euid: %d\n",getuid(),geteuid());
  if((file=fopen(argv[1],"r"))==NULL)
  {
    perror("fopen");
  }
  else
  {
    printf("file opened successfully\n");
    fclose(file);
  }

  setuid(getuid());
  printf("uid: %d, euid: %d\n",getuid(),geteuid());
  if((file=fopen(argv[1],"r"))==NULL)
  {
    perror("fopen");
  }
  else
  {
    printf("file opened successfully\n");
    fclose(file);
  }
  return EXIT_SUCCESS;
}
