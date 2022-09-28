#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

static void *thread_body(void *arg)
{
  int err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
  if(err!=0)
  {
    printf("pthread_setcancelstate: %s\n",strerror(err));
    return (void *)1;
  }
  for(;;)
  {
    printf("%d\n",rand());
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  srand(0);
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
  return 0;
}
