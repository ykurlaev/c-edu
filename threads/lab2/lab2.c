#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static void print_n(char *str, int count)
{
  for(int i=0; i<count; i++)
  {
    printf("%s\n",str);
  }
}

static void *thread_body(void *arg)
{
  static char new_str[] = "I'm a new thread";
  print_n(new_str,10);
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
  if(pthread_join(new_thread,NULL)!=0)
  {
    return EXIT_FAILURE;
  }
  print_n(parent_str,10);
  pthread_exit(NULL);
  return 0;
}
