#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct list
{
  struct list *next;
  char *data;
};

int create(struct list **head)
{
  *head=NULL;
  return 0;
}

int add(struct list **head,char *data)
{
  struct list *tmp=malloc(sizeof(struct list));
  if(!tmp)
  {
    return -1;
  }
  tmp->next=NULL;
  tmp->data=data;
  if(!*head)
  {
    *head=tmp;
  }
  else
  {
    struct list *p=*head;
    while(p->next!=NULL)
    {
      p=p->next;
    }
    p->next=tmp;
  }
  return 0;
}

int print(struct list **head)
{
  while(*head!=NULL)
  {
    printf("%s",(*head)->data);
    *head=(*head)->next;
  }
}

int destroy(struct list **head)
{
  while(*head!=NULL)
  {
    struct list *p=*head;
    *head=(*head)->next;
    free(p->data);
    free(p);
  }
  *head=NULL;
}

int read_line(char **buffer,size_t *buffer_size)
{
    char *buf=*buffer;
    size_t old_buffer_size=1;
    int eol_not_occured;
    do
    {
      eol_not_occured=0;
      fgets(buf,*buffer_size-old_buffer_size+1,stdin);
      if(strchr(*buffer,'\n')==NULL)
      {
        eol_not_occured=1;
        old_buffer_size=*buffer_size;
        *buffer_size*=2;
        char *p=realloc(*buffer,*buffer_size);
        if(p==NULL)
        {
          perror("realloc");
          return -1;
        }
        *buffer=p;
        buf=*buffer+old_buffer_size-1;
      }
    }
    while(eol_not_occured);
    return 0;
}

#define INITIAL_BUFFER_SIZE 32

int main()
{
  struct list *list;
  create(&list);
  size_t buffer_size=INITIAL_BUFFER_SIZE;
  char *buffer;
  if((buffer=malloc(buffer_size))==NULL)
  {
    perror("malloc");
    return EXIT_FAILURE;
  }
  int end=0;
  while(!end)
  {
    if(read_line(&buffer,&buffer_size)==-1)
    {
      destroy(&list);
      free(buffer);
      return EXIT_FAILURE;
    }
    end=(buffer[0]=='.');
    size_t length=strlen(buffer);
    char *string;
    if((string=malloc(length+1))==NULL)
    {
      destroy(&list);
      free(buffer);
      perror("malloc");
      return EXIT_FAILURE;
    }
    strcpy(string,buffer);
    if(add(&list,string)==-1)
    {
      destroy(&list);
      free(string);
      free(buffer);
      perror("add");
      return EXIT_FAILURE;
    }
  }
  print(&list);
  destroy(&list);
  free(buffer);
  return EXIT_SUCCESS;
}
