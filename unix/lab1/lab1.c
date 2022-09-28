#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/resource.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int option;
  char *options=":ispuU:cC:dvV:";
  struct rlimit rlim;
  char *buf=NULL;
  int size;
  extern char **environ;
  int i;
  char *delim=NULL;
  while((option=getopt(argc,argv,options))!=-1)
  {
    switch(option)
    {
      case 'i':
        printf("real uid is %d, effective uid is %d, real gid is %d, effective gid is %d\n",
                getuid(),geteuid(),getgid(),getegid());
        break;
      case 's':
        if(setpgid(0,0)==-1)
        {
          perror("setpgid failed");
        }
        else
        {
          printf("setpgid success\n");
        }
        break;
      case 'p':
        printf("process id is %d, parent id is %d, group id is %d\n",
                getpid(),getppid(),getpgrp());
        break;
      case 'u':
        printf("ulimit is %ld\n",ulimit(UL_GETFSIZE));
        break;
      case 'U':
        if(ulimit(UL_SETFSIZE,atol(optarg))==-1)
        {
          perror("ulimit failed");
        }
        else
        {
          printf("ulimit successfully changed\n");
        }
        break;
      case 'c':
        if(getrlimit(RLIMIT_CORE,&rlim)==-1)
        {
          perror("getrlimit failed");
        }
        else
        {
          printf("soft core file limit is %d, hard core file limit is %d\n",
                  rlim.rlim_cur,rlim.rlim_max);
        }
        break;
      case 'C':
        rlim.rlim_cur=atol(optarg);
        rlim.rlim_max=RLIM_SAVED_MAX;
        if(setrlimit(RLIMIT_CORE,&rlim)==-1)
        {
          perror("setrlimit failed");
        }
        else
        {
          printf("core file size successfully changed\n");
        }
        break;
      case 'd':
        size=pathconf(".", _PC_PATH_MAX);
        if((buf=malloc(size+1))==0)
        {
          perror("malloc failed");
        }
        else
        {
          if(getcwd(buf,size+1)==NULL)
          {
            perror("getcwd failed");
          }
          else
          {
            printf("current path is %s\n",buf);
          }
        }
        break;
      case 'v':
        for(i=0;environ[i]!=NULL;++i)
        {
          printf("%s\n",environ[i]);
        }
        break;
      case 'V':
        if((delim=strchr(optarg,'='))==NULL)
        {
          fprintf(stderr,"no '=' in V argument\n");
        }
        else
        {
          *delim='\0';
          delim++;
          if(setenv(optarg,delim,1)==-1)
          {
            perror("setenv failed");
          }
          else
          {
            printf("environment variable changed successfuly\n");
          }
        }
        break;
      case ':':
        fprintf(stderr,"Option %c require argument\n",optopt);
        break;
      case '?':
        fprintf(stderr,"Unknown option: %c\n",optopt);
        break;
      default:
        fprintf(stderr,"%c\n",option);
        break;
    }
  }
  return EXIT_SUCCESS;
}
