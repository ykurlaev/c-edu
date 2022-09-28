#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define INIT_TABLE_SIZE 32

off_t *build_offsets_table(int fd,unsigned *size)
{
  char *buffer;
  off_t *offsets_table;
  unsigned table_size=INIT_TABLE_SIZE;
  unsigned table_used=0;
  if((buffer=malloc(BUFFER_SIZE+1))==NULL ||
     (offsets_table=malloc(table_size*sizeof(off_t)))==NULL)
  {
    perror("Error allocataing memory");
    free(buffer);
    return NULL;
  }
  offsets_table[0]=0;
  table_used+=1;
  int bytes_read;
  unsigned iteration=0;
  do
  {
    bytes_read=read(fd,buffer,BUFFER_SIZE);
    if(bytes_read<0)
    {
      free(buffer);
      free(offsets_table);
      perror("Error reading file");
      return NULL;
    }
    buffer[bytes_read]='\0';
    char *position=buffer;
    while((position=strchr(position,'\n'))!=NULL)
    {
      position+=1;
      offsets_table[table_used]=position-buffer+iteration*BUFFER_SIZE;
      table_used+=1;
      if(table_used==table_size)
      {
        table_size*=2;
        off_t *p=realloc(offsets_table,table_size*sizeof(off_t));
        if(p==NULL)
        {
          free(buffer);
          free(offsets_table);
          perror("Error allocataing memory");
          return NULL;
        }
        offsets_table=p;
      }
    }
    iteration+=1;
  }
  while(bytes_read==BUFFER_SIZE);
  free(buffer);
  offsets_table[table_used]=(iteration-1)*BUFFER_SIZE+bytes_read;
  *size=table_used;
  return offsets_table;
}

int get_line_number(int table_size)
{
  int line_number;
  int correct;
  do
  {
    char buffer[101];
    int e;
    while((e=read(0,buffer,100))!=-1&&e!=0);
    if(e==-1&&errno!=EAGAIN&&errno!=EWOULDBLOCK)
    {
      perror("Error reading stdin");
      return -3;
    }
    printf("Enter line number in 5 seconds: ");
    fflush(stdout);
    sleep(5);
    if((e=read(0,buffer,100))==-1)
    {
      if(errno!=EAGAIN&&errno!=EWOULDBLOCK)
      {
        perror("Error reading stdin");
        return -3;
      }
      else
      {
        printf("\n");
        return -2;
      }
    }
    if(e==0)
    {
      printf("\n");
      return -2;
    }
    buffer[e]='\0';
    char *end;
    line_number=strtol(buffer,&end,10);
    correct=((*end=='\n')||(*end=='\0'))&&(line_number>=0)&&(line_number<table_size+1);
    line_number--;
    if(!correct)
    {
      printf("No such line!\n");
    }
  }
  while(!correct);
  return line_number;
}

char *get_line(int fd,unsigned line_number,off_t *offsets_table)
{
  unsigned line_length=offsets_table[line_number+1]-offsets_table[line_number];
  if(lseek(fd,offsets_table[line_number],SEEK_SET)==-1)
  {
    perror("Error seeking");
    return NULL;
  }
  char *buffer;
  if((buffer=malloc(line_length+1))==NULL)
  {
    perror("Error allocataing memory");
    return NULL;
  }
  int bytes_read;
  if((bytes_read=read(fd,buffer,line_length))<0)
  {
    free(buffer);
    perror("Error reading file");
    return NULL;
  }
  buffer[bytes_read]='\0';
  return buffer;
}

#ifdef DEBUG
void print_offsets_table(off_t *table,unsigned table_size)
{
  unsigned i;
  for(i=0;i<table_size;i++)
  {
    printf("%d: %d\n",i,table[i]);
  }
}
#endif

int print_all(int fd,off_t *offsets_table,unsigned table_size)
{
  unsigned line_number;
  for(line_number=0;line_number<table_size;line_number++)
  {
    char *line=get_line(fd,line_number,offsets_table);
    if(line==NULL)
    {
      return -1;
    }
    printf("%s",line);
    if(!strchr(line,'\n'))
    {
      printf("\n");
    }
    free(line);
  }
  return 0;
}

int main(int argc,char *argv[])
{
  if(argc!=2)
  {
    fprintf(stderr,"Usage: %s filename\n",argv[0]);
    return EXIT_FAILURE;
  }
  int fd;
  if((fd=open(argv[1],O_RDONLY))==-1)
  {
    perror("Error opening file");
    return EXIT_FAILURE;
  }
  unsigned table_size;
  off_t *offsets_table=build_offsets_table(fd,&table_size);
#ifdef DEBUG
  print_offsets_table(offsets_table,table_size);
#endif
  if(offsets_table==NULL)
  {
    if((close(fd))==-1)
    {
      perror("Error closing file");
    }
    return EXIT_FAILURE;
  }
  printf("File has %d lines\n",table_size);
  int flags,oldflags;
  if((oldflags=fcntl(0,F_GETFL))==-1)
  {
    perror("Error getting flags");
    if((close(fd))==-1)
    {
      perror("Error closing file");
    }
    return EXIT_FAILURE;
  }
  flags=oldflags|O_NDELAY;
  if((fcntl(0,F_SETFL,flags))==-1)
  {
    perror("Error setting flags");
    if((close(fd))==-1)
    {
      perror("Error closing file");
    }
    return EXIT_FAILURE;
  }
  int line_number;
  while((line_number=get_line_number(table_size))!=-1)
  {
    if(line_number==-3)
    {
      free(offsets_table);
      if((fcntl(0,F_SETFL,oldflags))==-1)
      {
        perror("Error setting flags");
      }
      if((close(fd))==-1)
      {
         perror("Error closing file");
      }
      return EXIT_FAILURE;
    }
    if(line_number==-2)
    {
      int e=0;
      e|=(print_all(fd,offsets_table,table_size)==-1);
      free(offsets_table);
      if((e|=(fcntl(0,F_SETFL,oldflags)==-1)))
      {
        perror("Error setting flags");
      }
      if((e|=(close(fd)==-1)))
      {
         perror("Error closing file");
      }
      if(e)
      {
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }
    char *line=get_line(fd,line_number,offsets_table);
    if(line==NULL)
    {
      free(offsets_table);
      if((fcntl(0,F_SETFL,oldflags))==-1)
      {
        perror("Error setting flags");
      }
      if((close(fd))==-1)
      {
         perror("Error closing file");
      }
      return EXIT_FAILURE;
    }
    printf("%s",line);
    if(!strchr(line,'\n'))
    {
      printf("\n");
    }
    free(line);
  }
  free(offsets_table);
  int e=0;
  if((e|=(fcntl(0,F_SETFL,oldflags)==-1)))
  {
    perror("Error setting flags");
  }
  if((e|=(close(fd)==-1)))
  {
    perror("Error closing file");
  }
  if(e)
  {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
