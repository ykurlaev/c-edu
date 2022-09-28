#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig)
{
  signal(SIGINT,handler);
  signal(SIGQUIT,handler);
  static long long count = 0;
  switch(sig)
  {
    case SIGINT:
      printf("\a");
      fflush(stdout);
      count++;
      break;
    case SIGQUIT:
      printf("\nYou have heared beep %lld times\n",count);
      exit(0);
      break;
  }
}

int main()
{
  signal(SIGINT,handler);
  signal(SIGQUIT,handler);
  for(;;)
  {
    pause();
  }
}
