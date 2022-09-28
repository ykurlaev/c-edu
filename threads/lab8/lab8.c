#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define ITERATIONS 10000

pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pi_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;

double pi = 0.;
unsigned num;
int exiting = 0;
int really_exiting = 0;

void *calc_body(void *arg)
{
  pthread_mutex_lock(&init_mutex);
  pthread_mutex_unlock(&init_mutex);
  unsigned k = (unsigned)arg;
  unsigned iter = 0;
  unsigned i = k;
  while(!really_exiting)
  {
    iter++;
    double pi_part = 0.;
    for(; i<iter*ITERATIONS+k; i+=num)
    {
      pi_part+=1.0/(i*4.0+1.0);
      pi_part-=1.0/(i*4.0+3.0);
    }
    pi_part*=4;
    pthread_barrier_wait(&barrier);
    pthread_mutex_lock(&pi_mutex);
    really_exiting = exiting;
    pi += pi_part;
#ifdef DEBUG
    fprintf(stderr,"Thread %d finished iteration %d. Partial sum is %lf. Pi now is %lf. Exiting is %d\n",k,iter,pi_part,pi,exiting);
#endif
    pthread_mutex_unlock(&pi_mutex);
    pthread_barrier_wait(&barrier);
  }
#ifdef DEBUG
  fprintf(stderr,"Thread %d exited.\n",k);
#endif
  return NULL;
}

void sigterm_handler(int signum)
{ 
  exiting = 1;
  signal(SIGINT,sigterm_handler);
}

int main(int argc, char *argv[])
{
  if(argc!=2)
  {
    fprintf(stderr,"Usage: %s thread_count\n",argv[0]);
    return 1;
  }
  signal(SIGINT,sigterm_handler);
  sscanf(argv[1],"%d",&num);
  int error = 0;
  pthread_t threads[num];
  unsigned i = 0;
  pthread_mutex_lock(&init_mutex);
  for(;i<num;i++)
  {
    int e = pthread_create(&threads[i],NULL,calc_body,(void *)i);
    if(e!=0)
    {
      printf("pthread_create: %s\n",strerror(e));
      break;
    }
  }
  num = i;
  pthread_barrier_init(&barrier,NULL,num);
  pthread_mutex_unlock(&init_mutex);
  for(unsigned j=0; j<num; j++)
  {
    int e = pthread_join(threads[j],NULL);
    if(e!=0)
    {
      printf("pthread_join: %s\n",strerror(e));
    }
  }
  printf("\nπ≈%.11lf\n",pi);
  pthread_barrier_destroy(&barrier);
  return 0;
}
