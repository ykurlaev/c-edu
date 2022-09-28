#include "queue.h"
#include <string.h>
#include <pthread.h>

struct node
{
  struct node *next;
  char data[81];
};

struct queue
{
  struct node *head;
  pthread_cond_t empty_cond;
  pthread_cond_t full_cond;
  pthread_mutex_t mutex;
  int drop;
  int count;
};

void mymsginit(queue *queue)
{
  *queue = malloc(sizeof(struct queue));
  (*queue)->drop = 0;
  (*queue)->head = NULL;
  int i;
  pthread_cond_init(&(*queue)->empty_cond, NULL);
  pthread_cond_init(&(*queue)->full_cond, NULL);
  pthread_mutex_init(&(*queue)->mutex, NULL);
}

void mymsgdrop(queue *queue)
{
  pthread_mutex_lock(&(*queue)->mutex);
  (*queue)->drop = 1;
  while((*queue)->head)
  {
    struct node *t = (*queue)->head;
    (*queue)->head = (*queue)->head->next;
    free(t);
  }
  pthread_cond_signal(&(*queue)->empty_cond);
  pthread_cond_signal(&(*queue)->full_cond);
  pthread_mutex_unlock(&(*queue)->mutex);
}

void mymsgdestroy(queue *queue)
{
  mymsgdrop(queue);
  pthread_mutex_lock(&(*queue)->mutex);
  pthread_cond_destroy(&(*queue)->empty_cond);
  pthread_cond_destroy(&(*queue)->full_cond);
  pthread_mutex_unlock(&(*queue)->mutex);
  pthread_mutex_destroy(&(*queue)->mutex); 
}

int mymsgput(queue *queue, char *msg)
{
  struct node *node = malloc(sizeof(struct node));
  strncpy(node->data, msg, 80);
  int r = strlen(node->data);
  node->next = NULL;
  pthread_mutex_lock(&(*queue)->mutex);
  while((*queue)->count==QUEUE_CAPACITY && !(*queue)->drop)
  {
    pthread_cond_wait(&(*queue)->full_cond, &(*queue)->mutex);
  }
  if((*queue)->drop)
  {
    pthread_cond_signal(&(*queue)->full_cond);
    pthread_mutex_unlock(&(*queue)->mutex);
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
  (*queue)->count++;
  pthread_cond_signal(&(*queue)->empty_cond);
  pthread_mutex_unlock(&(*queue)->mutex);
  return r;
}

int mymsgget(queue *queue, char *buf, size_t bufsize)
{
  struct node *node;
  pthread_mutex_lock(&(*queue)->mutex);
  while((*queue)->count==0 && !(*queue)->drop)
  {
    pthread_cond_wait(&(*queue)->empty_cond, &(*queue)->mutex);
  }
  if((*queue)->drop)
  {
    pthread_cond_signal(&(*queue)->empty_cond);
    pthread_mutex_unlock(&(*queue)->mutex);
    buf[0] = '\0';
    return 0;
  }
  node = (*queue)->head;
  (*queue)->head = node->next;
  (*queue)->count--;
  pthread_cond_signal(&(*queue)->full_cond);
  pthread_mutex_unlock(&(*queue)->mutex);
  strncpy(buf, node->data, bufsize);
  free(node);
  return strlen(buf);
}
