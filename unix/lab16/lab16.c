#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main()
{
  struct termios old_termios, new_termios;
  if(!isatty(STDIN_FILENO))
  {
    fprintf(stderr,"Error: stdin is not a terminal!\n");
    return EXIT_FAILURE;
  }
  if(tcgetattr(STDIN_FILENO,&old_termios)==-1)
  {
    perror("tcgetattr");
    return EXIT_FAILURE;
  }
  new_termios=old_termios;
  new_termios.c_lflag&=~(ISIG|ICANON);
  new_termios.c_cc[VMIN]=1;
  new_termios.c_cc[VTIME]=0;
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&new_termios)==-1)
  {
    perror("tcsetattr");
    return EXIT_FAILURE;
  }
  while(1)
  {
    printf("Do you want to reinstall Shindows? (y/n) ");
    fflush(stdout);
    char answer;
    if(read(STDIN_FILENO,&answer,1)!=1)
    {
      if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&old_termios)==-1)
      {
        perror("rcsetattr");
      }
      return EXIT_FAILURE;
    }
    if(answer=='y')
    {
      printf("\nCan't reinstall Shindows! Please, reinstall Shindows.\n");
      break;
    }
    else if(answer=='n')
    {
      printf("\nOkay.\n");
      break;
    }
    else
    {
      printf("\nIncorrect answer! Try again.\n");
    }
  }
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&old_termios)==-1)
  {
    perror("rcsetattr");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
