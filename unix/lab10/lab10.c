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
    if(execvp(argv[1],argv+1)==-1)
    {
      perror("execvp");
      return 1;
    }
  }
  else
  {
    if(pid==-1)
    {
      perror("fork");
      return EXIT_FAILURE;
    }
    int status;
    if(waitpid(pid,&status,0)==-1)
    {
      perror("waitpid");
    }
    if(WIFEXITED(status))
    {
      printf("Program finished normally with exit status %d\n",WEXITSTATUS(status));
    }
    else
    {
      printf("Program finished abnormally with signal %d\n",WTERMSIG(status));
    }
  }
  return EXIT_SUCCESS;
}
