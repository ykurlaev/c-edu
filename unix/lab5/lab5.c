#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    printf("Enter line number: ");
    int e=scanf("%d",&line_number);
    while((getchar())!='\n');
    correct=(e==1)&&(line_number>=0)&&(line_number<table_size+1);
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
  lseek(fd,offsets_table[line_number],SEEK_SET);
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
  int line_number;
  while((line_number=get_line_number(table_size))!=-1)
  {
    char *line=get_line(fd,line_number,offsets_table);
    if(line==NULL)
    {
      free(offsets_table);
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
  if((close(fd))==-1)
  {
    perror("Error closing file");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
