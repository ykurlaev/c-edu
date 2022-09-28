#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutexes[3] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

static void print_n(char *str, int count, int first_mutex, int second_mutex)
{
  for(int i=0; i<count; i++)
  {
#ifndef NDEBUG
    fprintf(stderr,"Thread %d, iteration %d: first mutex %d, second mutex %d\n",(int)str,i,first_mutex,second_mutex);
#endif
    pthread_mutex_lock(&mutexes[second_mutex]);
#ifndef NDEBUG
    fprintf(stderr,"Thread %d locked mutex %d\n",(int)str,second_mutex);
#endif
#ifndef NDEBUG
    fprintf(stderr,"%d: ",(int)str);
#endif
    printf("%s\n",str);
    pthread_mutex_unlock(&mutexes[first_mutex]);
#ifndef NDEBUG
    fprintf(stderr,"Thread %d unlocked mutex %d\n",(int)str,first_mutex);
#endif
    first_mutex=second_mutex;
    second_mutex=(second_mutex+1)%3;
  }
  pthread_mutex_unlock(&mutexes[first_mutex]);
#ifndef NDEBUG
  fprintf(stderr,"Thread %d unlocked mutex %d\n",(int)str,first_mutex);
#endif
}

static void *thread_body(void *arg)
{
  static char new_str[] = "I'm a new thread";
  pthread_mutex_lock(&mutexes[2]);
#ifndef NDEBUG
  fprintf(stderr,"Thread %d locked mutex %d\n",(int)new_str,2);
#endif
  print_n(new_str,10,2,0);
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
  pthread_mutex_lock(&mutexes[0]);
#ifndef NDEBUG
  fprintf(stderr,"Thread %d locked mutex %d\n",(int)parent_str,0);
#endif
  print_n(parent_str,10,0,1);
  pthread_exit(NULL);
  return 0;
}
