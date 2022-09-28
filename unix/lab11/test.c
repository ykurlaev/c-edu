#include <stdio.h>

extern char **environ;

int main(int argc,char *argv[])
{
  char **p;
  int n;
  printf("input parameters:\n");
  for(n=0;n<argc;n++)
    printf("    %2d: '%s'\n",n,argv[n]);
  printf("environment variables:\n");
  for (p = environ;*p!=0;p++)
    printf("    %s\n",*p);
  return 0;
}