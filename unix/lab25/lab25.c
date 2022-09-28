#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 16

int main()
{
  int fds[2];
  if(pipe(fds)==-1)
  {
    perror("pipe");
    return 1;
  }
  pid_t pid = fork();
  if(pid==-1)
  {
    perror("pipe");
    close(fds[0]);
    close(fds[1]);
    return 1;
  }
  int error = 0;
  char buffer[BUFFER_SIZE+1];
  int in, out;
  if(pid)
  {
    close(fds[0]);
    in = STDIN_FILENO;
    out = fds[1];
  }
  else
  {
    close(fds[1]);
    in = fds[0];
    out = STDOUT_FILENO;
  }
  for(;;)
  {
    ssize_t e = read(in,buffer,BUFFER_SIZE);
    if(e==-1)
    {
      perror("read");
      error=1;
      break;
    }
    if(e==0)
    {
      break;
    }
    buffer[e]='\0';
    if(!pid)
    {
      for(int i=0;i<e;i++)
      {
        buffer[i] = toupper(buffer[i]);
      }
    }
    e = write(out,buffer,e);
    if(e==-1)
    {
      perror("write");
      error=1;
      break;
    }
  }
  close(in);
  close(out);
  return error;
}
