#include <stdio.h>
#include <stdlib.h>

struct dlist
{
  unsigned int data;
  struct dlist *prev,*next;
};

struct dlist *init(unsigned int n)
{
  unsigned int i;
  struct dlist *head,*tail,*p;
  head=tail=p=malloc(sizeof(struct dlist));
  p->prev=p->next=NULL;
  p->data=1;
  for(i=2;i<=n;i++)
  {
    p=malloc(sizeof(struct dlist));
    p->data=i;
    p->next=NULL;
    p->prev=tail;
    tail->next=p;
    tail=p;
  }
  tail->next=head;
  head->prev=tail;
  return head;
}

int main()
{
  struct dlist *p,*t;
  unsigned int n,n2,i;
  printf("Enter the number of people: ");
  scanf("%d",&n);
  printf("Enter killing step: ");
  scanf("%d",&n2);
  p=init(n);
  while(p->prev!=p)
  {
    for(i=1;i<n2;i++)
    {
      p=p->next;
    }
    p->prev->next=p->next;
    p->next->prev=p->prev;
    t=p->next;
    free(p);
    p=t;
  }
  printf("Suvirved person is number %d!\n",p->data);
  return 0;
}
