#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>

sem_t *sems[2];

static void print_n(char *str, int count, int me, int notme)
{
  for(int i=0; i<count; i++)
  {
    sem_wait(sems[notme]);
    printf("%s\n",str);
    sem_post(sems[me]);
  }
}

int main(int argc, char *argv)
{
  static char parent_str[] = "I'm a parent process";
  static char new_str[] = "I'm a new process";
  int i;
  char a[2][50];
  sprintf(a[0],"/qwerty1%d%d",getpid(),(int)time(NULL));
  sprintf(a[1],"/qwerty2%d%d",getpid(),(int)time(NULL));
  for(i=0;i<2;i++)
  {
    if((sems[i] = sem_open(a[i],O_CREAT,0700,i))!=SEM_FAILED)
    {
      fprintf(stderr,"%d ",i);
      perror("sem_open");
      return 1;
    }
  }
  int e = fork();
  if(e<0)
  {
    perror("fork");
    return 1;
  }
  if(e)
  {
    print_n(parent_str,10,0,1);
  }
  else
  {
    print_n(new_str,10,1,0);
    for(i=0;i<2;i++)
    {
     if(sem_close(sems[i]))
     {
       perror("sem_init");
       return 1;
     }
    }
  }
  pthread_exit(NULL);
  return 0;
}
