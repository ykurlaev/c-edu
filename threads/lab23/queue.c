#include "queue.h"
#include <string.h>
#include <semaphore.h>

struct node
{
  struct node *next;
  char data[81];
};

struct queue
{
  struct node *head;
  sem_t empty_sem;
  sem_t full_sem;
  sem_t mutex_sem;
  int drop;
};

void mymsginit(queue *queue)
{
  *queue = malloc(sizeof(struct queue));
  (*queue)->drop = 0;
  (*queue)->head = NULL;
  int i;
  sem_init(&(*queue)->empty_sem, 0, 0);
  sem_init(&(*queue)->full_sem, 0, QUEUE_CAPACITY);
  sem_init(&(*queue)->mutex_sem, 0, 1);
}

void mymsgdrop(queue *queue)
{
  sem_wait(&(*queue)->mutex_sem);
  (*queue)->drop = 1;
  while((*queue)->head)
  {
    struct node *t = (*queue)->head;
    (*queue)->head = (*queue)->head->next;
    free(t);
  }
  sem_post(&(*queue)->empty_sem);
  sem_post(&(*queue)->full_sem);
  sem_post(&(*queue)->mutex_sem);
}

void mymsgdestroy(queue *queue)
{
  mymsgdrop(queue);
  sem_wait(&(*queue)->mutex_sem);
  sem_destroy(&(*queue)->empty_sem);
  sem_destroy(&(*queue)->full_sem);
  sem_destroy(&(*queue)->mutex_sem);
}

int mymsgput(queue *queue, char *msg)
{
  struct node *node = malloc(sizeof(struct node));
  strncpy(node->data, msg, 80);
  int r = strlen(node->data);
  node->next = NULL;
  sem_wait(&(*queue)->full_sem);
  sem_wait(&(*queue)->mutex_sem);
  if((*queue)->drop)
  {
    sem_post(&(*queue)->full_sem);
    sem_post(&(*queue)->mutex_sem);
    return 0;
  }
  if((*queue)->head)
  {
    struct node *t = (*queue)->head;
    while(t->next)
    {
      t = t->next;
    }
    t->next = node;
  }
  else
  {
    (*queue)->head = node;
  }
  sem_post(&(*queue)->mutex_sem);
  sem_post(&(*queue)->empty_sem);
  return r;
}

int mymsgget(queue *queue, char *buf, size_t bufsize)
{
  struct node *node;
  sem_wait(&(*queue)->empty_sem);
  sem_wait(&(*queue)->mutex_sem);
  if((*queue)->drop)
  {
    sem_post(&(*queue)->empty_sem);
    sem_post(&(*queue)->mutex_sem);
    buf[0] = '\0';
    return 0;
  }
  node = (*queue)->head;
  (*queue)->head = node->next;
  sem_post(&(*queue)->mutex_sem);
  sem_post(&(*queue)->full_sem);
  strncpy(buf, node->data, bufsize);
  free(node);
  return strlen(buf);
}
