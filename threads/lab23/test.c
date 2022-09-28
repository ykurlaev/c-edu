#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "pthread.h"

queue q;
pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;

void *producer_thread(void *arg)
{
  pthread_mutex_lock(&init_mutex);
  pthread_mutex_unlock(&init_mutex);
  int c = (int)arg;
  char str[10];
  int i;
  for(i=0; i<20; i++)
  {
    printf("Producer %d producing: %d\n", c, i);
    sprintf(str, "%d - %d", c, i);
    mymsgput(&q, str);
    if(i==10 && c==0)
    {
      pthread_barrier_wait(&barrier);
    }
  }
  return NULL;
}

void *consumer_thread(void *arg)
{
  int c = (int)arg;
  char str[10];
  int i;
  for(i=0; i<20; i++)
  {
    mymsgget(&q, str, 10);
    printf("Consumer %d consumed \"%s\"\n", c, str);
  }
  return NULL;
}

int main(int argc, char *argv)
{
  pthread_barrier_init(&barrier, NULL, 2);
  mymsginit(&q);
  pthread_t producers[2];
  pthread_t consumers[2];
  int i;
  pthread_mutex_lock(&init_mutex);
  for(i=0; i<2; i++)
  {
    pthread_create(&producers[i], NULL, producer_thread, (void *)i);
    pthread_create(&consumers[i], NULL, consumer_thread, (void *)i);
  }
  pthread_mutex_unlock(&init_mutex);
  pthread_barrier_wait(&barrier);
  printf("DROP!\n");
  mymsgdrop(&q);
  for(i=0; i<2; i++)
  {
    pthread_join(producers[i], NULL);
    pthread_join(consumers[i], NULL);
  }
  mymsgdestroy(&q);
  return 0;
}
