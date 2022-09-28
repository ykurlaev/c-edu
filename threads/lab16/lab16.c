#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

struct node
{
  struct node *next;
  char *str;
};

struct list
{
  struct node pre_head;
  struct node *head;
  pthread_mutex_t mutex;
};

void list_init(struct list *list)
{
  list->head = NULL;
  list->pre_head.next = list->head;
  list->pre_head.str = NULL;
  pthread_mutex_init(&list->mutex,NULL);
}

int list_add(struct list *list, char *str)
{
  struct node *node = malloc(sizeof(struct node));
  if(node==NULL)
  {
    return -1;
  }
  pthread_mutex_lock(&list->mutex);
  node->next = list->head;
  node->str = strdup(str);
  if(node->str==NULL)
  {
    free(node);
    pthread_mutex_unlock(&list->mutex);
    return -1;
  }
  list->head = node;
  list->pre_head.next = list->head;
  pthread_mutex_unlock(&list->mutex);
  return 0;
}

void list_dispose(struct list *list)
{
  pthread_mutex_lock(&list->mutex);
  while(list->head!=NULL)
  {
    struct node *node = list->head;
    list->head = list->head->next;
    free(node->str);
    free(node);
  }
  list->pre_head.next = list->head;
  pthread_mutex_unlock(&list->mutex);
}

void list_sort(struct list *list)
{
  pthread_mutex_lock(&list->mutex);
  if(list->head==NULL)
  {
    pthread_mutex_unlock(&list->mutex);
    return;
  }
  struct node *i;
  int sorting = 1;
  while(sorting)
  {
    sorting = 0;
    for(i=&list->pre_head; i->next->next!=NULL; i=i->next)
    {
      struct node *j = i->next->next;
      if(strcmp(i->next->str,j->str)>0)
      {
        struct node *tmp = j->next;
        j->next = i->next;
        i->next->next = tmp;
        i->next = j;
        sorting = 1;
      }
    }
  }
  list->head = list->pre_head.next;
  pthread_mutex_unlock(&list->mutex);
}

void list_print(struct list *list)
{
  pthread_mutex_lock(&list->mutex);
  struct node *i;
  for(i=list->head; i!=NULL; i=i->next)
  {
    fputs(i->str,stdout);
  }
  pthread_mutex_unlock(&list->mutex);
}

void *sorting_thread_body(void *arg)
{
  struct list *list = (struct list *)arg;
  for(;;)
  {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
    list_sort(list);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    sleep(5);
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  struct list list;
  list_init(&list);
  pthread_t sorting_thread;
  int e;
  if((e=pthread_create(&sorting_thread,NULL,sorting_thread_body,&list))!=0)
  {
    printf("Can't create thread: %s\n",strerror(e));
    return 1;
  }
  static char buf[82];
  while(fgets(buf,80,stdin)!=NULL)
  {
    if(strchr(buf,'\n')==NULL)
    {
      buf[strlen(buf)]='\n';
      buf[strlen(buf)+1]='\0';
    }
    if(strcmp(buf,"\n")==0)
    {
      list_print(&list);
    }
    else
    {
      if(list_add(&list,buf)!=0)
      {
        perror("Can't allocate memory");
        break;
      }
    }
  }
  if((e=pthread_cancel(sorting_thread))!=0)
  {
    printf("Can't cancel thread: %s\n",strerror(e));
    return 1;
  }
  if((e=pthread_join(sorting_thread,NULL))!=0)
  {
    printf("Can't join thread: %s\n",strerror(e));
    return 1;
  }
  list_dispose(&list);
  return 0;
}
