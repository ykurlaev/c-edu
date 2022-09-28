#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


static void *thread_body(void *arg)
{
  char **string = (char **)arg;
  for(int i=0;i<3;i++)
  {
    printf("%s\n",*string);
    string++;
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  static char *strings[][3] = {{"ONE_ONE", "ONE_TWO", "ONE_THREE"},
                              {"TWO_ONE", "TWO_TWO", "TWO_THREE"},
                              {"THREE_ONE", "THREE_TWO", "THREE_THREE"},
                              {"FOUR_ONE", "FOUR_TWO", "FOUR_THREE"}};
  pthread_t threads[4];
  int error = 0;
  int i = 0;
  for(;i<4;i++)
  {
    int err = pthread_create(&threads[i],NULL,thread_body,strings[i]);
    if(err!=0)
    {
      printf("pthread_create: %s\n",strerror(err));
      error = 1;
      break;
    }
  }
  for(int j=0;j<i;j++)
  {
    int err = pthread_join(threads[j],NULL);
    if(err!=0)
    {
      printf("pthread_join: %s\n",strerror(err));
      error = 1;
    }
    if(error!=0)
    {
      return EXIT_FAILURE;
    }
  }
  return 0;
}
