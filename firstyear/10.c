#include <stdio.h>
#include <stdlib.h>

struct node {
  int data;
  struct node *next;
};
  
struct node * insert(struct node *s,int a)
{
  struct node *t,*h;
  h=s;
  int first=1;
  while(s)
  {
    if(first&&s->data>a)
    {
      t=(struct node*)malloc(sizeof(struct node));
      t->data=a;
      t->next=s;
      s=t;
      return s;
    }
    if(!s->next&&s->data<a)
    {
      s->next=(struct node*)malloc(sizeof(struct node));
      s->next->data=a;
      s->next->next=NULL;
      return h;
    }
    if(s->data<a&&s->next->data>a)
    {
      t=(struct node*)malloc(sizeof(struct node));
      t->data=a;
      t->next=s->next;
      s->next=t;
      return h;
    }
    first=0;
    s=s->next;
  }
  return h;
}

void printlist(struct node *s)
{
  while(s)
  {
    printf("%d ",s->data);
    s=s->next;
  }
  printf("\n");
}

int main()
{
  struct node *head;
  int tmp;
  scanf("%d",&tmp);
  head=(struct node*)malloc(sizeof(struct node));
  head->data=tmp;
  head->next=NULL;
  while(1)
  {
    scanf("%d",&tmp);
    if(tmp==0)
      break;
    head=insert(head,tmp);
  }
  printlist(head);
  return 0;
}
