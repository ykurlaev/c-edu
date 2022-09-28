#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char author[256],year[5],title[256],comment[512];
} book;

void PrintHelp()
{
  printf("Allowed commands:\n");
  printf("'q' - exit;\n");
  printf("'h' - print this help;\n");
  printf("'l' - list books in library;\n");
  printf("'a' - add book to lybrary;\n");
  printf("'s' - search book;\n");
  printf("'d' - clear library.\n");
  return;
}

int FindBooks(char *name,char *substr)
{
  int c=0,i;
  book tmp,tmp2;
  FILE *lib;
  lib=fopen(name,"rb");
  if(!lib)
    return -1;
  while(fread(&tmp,sizeof(tmp),1,lib))
  {
    if(strstr(tmp.author,substr)||strstr(tmp.title,substr)||strstr(tmp.year,substr)||strstr(tmp.comment,substr))
    {
      printf("Author: %s  Title: %s  Year: %s\n  Comment: %s",tmp.author,tmp.title,tmp.year,tmp.comment);
      c++;
    }
  }  
  return c;
}

int WriteBook(char *name)
{
  FILE *lib;
  char s[256],s1[512],y[5];
  int d,f;
  lib=fopen(name,"ab");
  book tmp;
  printf("Enter author: ");
  strncpy(s,fgets(s,256,stdin),256);
  strncpy(tmp.author,s,256);
  printf("Enter title: ");
  strncpy(s,fgets(s,256,stdin),256);
  strncpy(tmp.title,s,256);
  do
  {
    printf("Enter year: ");
    f=scanf("%d",&d);
    if(f==1&&d>-9999&&d<9999)
    {
      sprintf(y,"%d",d);
      strncpy(tmp.year,y,5);
    }
    else
    {
      printf("Incorrect year! Try again.\n");
      if(d<-9999||d>9999)
	printf("Year must be beetween -9999 and 9999.\n");
      f=0;
    }
    strncpy(s,fgets(s,256,stdin),256); //some dirty hacks
  }
  while(f!=1);
  printf("Enter comment: ");
  strncpy(s1,fgets(s1,512,stdin),512);
  strncpy(tmp.comment,s1,512);
  fwrite(&tmp,sizeof(tmp),1,lib);
  fclose(lib);
  return 1;
}

int ListBooks(char *name)
{
  int c=0;
  book tmp;
  FILE *lib;
  lib=fopen(name,"rb");
  if(!lib)
    return -1;
  while(fread(&tmp,sizeof(tmp),1,lib))
  {
    c++;
    printf("Book number %d\n  Author: %s  Title: %s  Year: %s\n  Comment: %s",c,tmp.author,tmp.title,tmp.year,tmp.comment);
  }
  fclose(lib);
  return c;
}

int Extriminatus(char *name)
{
  FILE *lib;
  lib=fopen(name,"wb");
  fclose(lib);
  printf("Library is empty!\n");
  return 1;
} 

int main()
{
  char c[256];
  int exit=0,e;
  char name[256];
  strcpy(name,"lib.dat");
  printf("Enter 'h' for help.\n");
  do
  {
    strcpy(fgets(c,256,stdin),c);
    switch(c[0])
    {
      case 'q':
	exit=1;
	break;
      case 'a':
	WriteBook(name);
	printf("Book is added.\n");
	break;
      case 's':
	printf("Enter something to find.\n");
	scanf("%s",c);
	e=FindBooks(name,c);
	if(e<0)
	  printf("Some error accured!\n");
	else
	  printf("%d books found.\n",e);
	strcpy(fgets(c,256,stdin),c); //it's hack too!
	break;
      case 'l':
	e=ListBooks(name);
	if(e<0)
	  printf("Some error accured!\n");
	else
	  printf("%d books readen.\n",e);
	break;
      case 'd':
	printf("All data will be erased!\nAre you sure? (Enter 'y' to confirm): ");
	strcpy(fgets(c,256,stdin),c);
	if(c[0]=='y')
	  Extriminatus(name);
	break;
      default:
	printf("Unknown command. ");
      case 'h':
	PrintHelp();
	break;
    }
  }
  while(!exit);
  return 0;
}
