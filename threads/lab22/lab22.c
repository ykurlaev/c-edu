#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t A_sem, B_sem, AB_sem, C_sem;

void *A(void *arg)
{
  for(;;)
  {
    sleep(1);
    fprintf(stderr,"A\n");
    sem_post(&A_sem);
  }
}

void *B(void *arg)
{
  for(;;)
  {
    sleep(2);
    fprintf(stderr,"B\n");
    sem_post(&B_sem);
  }
}

void *AB(void *arg)
{
  for(;;)
  {
    sem_wait(&A_sem);
    sem_wait(&B_sem);
    fprintf(stderr,"AB\n");
    sem_post(&AB_sem);
  }
}

void *C(void *arg)
{
  for(;;)
  {
    sleep(3);
    fprintf(stderr,"C\n");
    sem_post(&C_sem);
  }
}

void *widget(void *arg)
{
  for(;;)
  {
    sem_wait(&AB_sem);
    sem_wait(&C_sem);
    printf("Widget!\n");
  }
}

int main(int argc, char *argv)
{
  pthread_t threads[5];
  void *(*bodies[5])(void *) = {A, B, AB, C, widget};
  sem_t *sems[4] = {&A_sem, &B_sem, &AB_sem, &C_sem};
  int i;
  for(i=0; i<4; i++)
  {
    sem_init(sems[i], 0, 0);
  }
  for(i=0; i<5; i++)
  {
    pthread_create(&threads[i],NULL,bodies[i],NULL);
  }
  for(i=0; i<5; i++)
  {
    pthread_join(threads[i],NULL);
  }
  return 0;
}
