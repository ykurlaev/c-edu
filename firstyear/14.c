#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tree
{
  char str[256]; //a lot of memory!
  struct tree *l,*r;
};

void out(struct tree *head)
{
  out(head->l);
  printf("%s",head->str);
  out(head->r);
}

int main()
{
  char tmp[256];
  int f;
  struct tree *head,*p,*t;
  fgets(tmp,256,stdin);
  head=malloc(sizeof(struct tree));
  strcpy(head->str,tmp);
  head->l=head->r=NULL;
  fgets(tmp,256,stdin);
  while(!strcmp(tmp,"\n"))
  {
    p=head;
    do
    {
      t=p;
      f=strcmp(tmp,t->str);
      if(f<0)
      {
	p=p->l;
      }
      else
      {
	p=p->r;
      }
    }
    while(p);
    p=malloc(sizeof(struct tree));
    strcpy(p->str,tmp);
    if(f<0)
      t->l=p;
    else
      t->r=p;
    fgets(tmp,256,stdin);
  }
  out(head);
  return 0;
}
