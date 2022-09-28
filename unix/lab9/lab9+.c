#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
  if(argc<2)
  {
    fprintf(stderr,"Invalid argument\n");
    return EXIT_FAILURE;
  }
  pid_t pid=fork();
  if(pid==0)
  {
    if(execvp("cat",argv)==-1)
    {
      perror("exec");
      return EXIT_FAILURE;
    }
  }
  else
  {
    if(pid<0)
    {
      perror("fork");
      return EXIT_FAILURE;
    }
    if(wait(NULL)==-1)
    {
      perror("wait");
      return EXIT_FAILURE;
    }
    printf("some string\n");
    return EXIT_SUCCESS;
  }
}
