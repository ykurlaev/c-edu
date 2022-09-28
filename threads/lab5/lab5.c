#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

static void thread_canceled(void *arg)
{
  printf("Canceled!\n");
}

static void *thread_body(void *arg)
{
  pthread_cleanup_push(thread_canceled,NULL);
  srand(0);
  for(;;)
  {
    printf("%d\n",rand());
  }
  pthread_cleanup_pop(0);
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t thread;
  int err = pthread_create(&thread,NULL,thread_body,NULL);
  if(err!=0)
  {
    printf("pthread_create: %s\n",strerror(err));
    return EXIT_FAILURE;
  }
  sleep(2);
  printf("Canceling...\n");
  err = pthread_cancel(thread);
  if(err!=0)
  {
    printf("pthread_cancel: %s\n",strerror(err));
    return EXIT_FAILURE;
  }
  err = pthread_join(thread,NULL);
  if(err!=0)
  {
    printf("pthread_join: %s\n",strerror(err));
    return EXIT_FAILURE;
  }
  pthread_exit(NULL);
  return 0;
}
