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
    char *envp[]={"FIRST=ONE","SECOND=TWO","THIRD=THREE",NULL};
    if(execvpe(argv[1],argv+1,envp)==-1)
    {
      perror("execvpe");
      return 1;
    }
  }
  else
  {
    if(pid<0)
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
      printf("Program finished abnormally\n");
    }
  }
  return EXIT_SUCCESS;
}

