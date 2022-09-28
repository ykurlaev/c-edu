#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_condition = PTHREAD_COND_INITIALIZER;
int current = 1;

static void print_n(char *str, int count, int notme)
{
  for(int i=0; i<count; i++)
  {
    pthread_mutex_lock(&ready_mutex);
    while(current==notme)
    {
      pthread_cond_wait(&ready_condition,&ready_mutex);
    }
    pthread_mutex_unlock(&ready_mutex);
    printf("%s\n",str);
    pthread_mutex_lock(&ready_mutex);
    current = notme;
    pthread_cond_signal(&ready_condition);
    pthread_mutex_unlock(&ready_mutex);
  }
}

static void *thread_body(void *arg)
{
  static char new_str[] = "I'm a new thread";
  print_n(new_str,10,1);
  return NULL;
}

int main(int argc, char *argv)
{
  static char parent_str[] = "I'm a parent thread";
  pthread_t new_thread;
  if(pthread_create(&new_thread,NULL,thread_body,NULL)!=0)
  {
    return EXIT_FAILURE;
  }
  print_n(parent_str,10,0);
  pthread_exit(NULL);
  return 0;
}
