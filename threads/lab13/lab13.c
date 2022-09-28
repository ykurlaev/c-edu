#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sems[2];

static void print_n(char *str, int count, int me, int notme)
{
  for(int i=0; i<count; i++)
  {
    sem_wait(&sems[notme]);
    printf("%s\n",str);
    sem_post(&sems[me]);
  }
}

static void *thread_body(void *arg)
{
  static char new_str[] = "I'm a new thread";
  print_n(new_str,10,1,0);
  return NULL;
}

int main(int argc, char *argv)
{
  static char parent_str[] = "I'm a parent thread";
  int i;
  for(i=0;i<2;i++)
  {
    if(sem_init(&sems[i],0,i))
    {
      perror("sem_init");
      return 1;
    }
  }
  pthread_t new_thread;
  if(pthread_create(&new_thread,NULL,thread_body,NULL)!=0)
  {
    return EXIT_FAILURE;
  }
  print_n(parent_str,10,0,1);
  for(i=0;i<2;i++)
  {
    if(sem_destroy(&sems[i]))
    {
      perror("sem_init");
      return 1;
    }
  }
  pthread_exit(NULL);
  return 0;
}
