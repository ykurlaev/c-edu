#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define ITERATIONS 10000000

int num;

void *calc_body(void *arg)
{
  int k = (int)arg;
  double *pi = malloc(sizeof(double));
  if(pi==NULL)
  {
    perror("malloc");
    return NULL;
  }
  *pi = 0;
  for(int i=k;i<ITERATIONS+k;i+=num)
  {
    *pi+=1.0/(i*4.0+1.0);
    *pi-=1.0/(i*4.0+3.0);
  }
  *pi*=4;
#ifndef NDEBUG
  printf("Thread %d: %lf\n",k,*pi);
#endif
  return (void *)pi;
}

int main(int argc, char *argv[])
{
  scanf("%d",&num);
  int error = 0;
  pthread_t threads[num];
  int i = 0;
  for(;i<num;i++)
  {
    int e = pthread_create(&threads[i],NULL,calc_body,(void *)i);
    if(e!=0)
    {
      printf("pthread_create: %s\n",strerror(e));
      error = 1;
      break;
    }
  }
  double result = 0;
  for(int j=0;j<i;j++)
  {
    void *p;
    int e = pthread_join(threads[j],&p);
    if(e!=0)
    {
      printf("pthread_join: %s\n",strerror(e));
      error = 1;
      free(p);
    }
    if(p==NULL)
    {
      error=1;
    }
    if(!error)
    {
      result+=*(double *)p;
      free(p);
    }
  }
  if(!error)
  {
    printf("π≈%.7lf\n",result);
  }
  else
  {
    printf("An error occured!\n");
  }
  return 0;
}
