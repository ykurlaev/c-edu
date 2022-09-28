 #include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc,char *argv[])
{
  if(argc!=2)
  {
    fprintf(stderr,"Usgae: %s filename\n",argv[0]);
    return EXIT_FAILURE;
  }
  int fd=open(argv[1],O_RDWR);
  struct flock lock;
  lock.l_type=F_WRLCK;
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0;
  if(fcntl(fd,F_SETLK,&lock)==-1)
  {
    if(errno==EAGAIN || errno==EACCES)
    {
      fprintf(stderr,"File is busy, try again later\n");
    }
    else
    {
      perror("Can't lock file");
    }
    if(close(fd)==-1)
    {
      perror("Can't close file");
    }
    return EXIT_FAILURE;
  }
  char command[5+strlen(argv[1])];
  sprintf(command,"vim %s",argv[1]);
  system(command);
  if(close(fd)==-1)
  {
    perror("Can't close file");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
