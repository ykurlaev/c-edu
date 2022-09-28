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
    execvp("cat",argv);
  }
  else
  {
    if(pid<0)
    {
      perror("fork");
      return EXIT_FAILURE;
    }
    printf("some string");
  }
  return EXIT_SUCCESS;
}
