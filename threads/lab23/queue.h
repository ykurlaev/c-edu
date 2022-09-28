#include <stdlib.h>

struct queue;

typedef struct queue *queue;

#define QUEUE_CAPACITY 10

void mymsginit(queue *queue); 
void mymsgdrop(queue *queue);
void mymsgdestroy(queue *queue);
int mymsgput(queue *queue, char *msg);
int mymsgget(queue *queue, char * buf, size_t bufsize);
