#include <stdio.h>
#include <string.h>
#include <math.h>
#define ERR '@'

char AtI(char A)
{
 if(('0'<=A)&&(A<='9'))
  return A-'0';
 if(('A'<=A)&&(A<='Z'))
  return A-'A'+10;
 if(('a'<=A)&&(A<='z'))
  return A-'a'+10;
 return ERR;
}

char ItA(char I)
{
 if((0<=I)&&(I<=9))
  return I+'0';
 if((10<=I)&&(I<=36))
  return I-10+'A';
 return ERR;
}

float StFn(char* s,char n)
{
 float res=0,resi=0;
 char a;
 char* dot;
 int l,i;
 char minus=0;
 if(dot=strchr(s,'.'))
 {
  l=strlen(dot);
  for(i=l-1;i>0;i--)
  {
   a=AtI(dot[i]);
   if(a==ERR||a>n)
    return ERR;
   res+=a;
   res/=n;
  }
  l=strcspn(s,".");
 }
 else
 {
  l=strlen(s);
 }
 if(s[0]=='-')
 {
  minus=1;
  s++;
  l--;
 }
 if(AtI(s[0])==ERR)
  return ERR;
 for(i=0;i<=l-1;i++)
 {
  a=AtI(s[i]);
  if(a==ERR||a>=n)
   return ERR;
/*  if(a==ERR&&i!=l-1&&s[i]!='-')
   return ERR;
  if(i==0&&s[i]=='-')
  {
   minus=1;
   break;
  }*/
  resi*=n;
  resi+=a;
 }
 res+=resi;
 if(minus)
  res*=-1;
 return res;
}

char* FtSn(float f,char n)
{
 int l;
 char res[255]="",m[255]="",c;
 char aftdot[11]="",minus=0,dot=1,d[11]=".";
 int a;
 if(f<0)
 {
  minus=1;
  f*=-1;
 }
 l=(int)(log(f)/log(n));
// res=(char*)calloc(l,1);
 a=(int)f;
// l-=2;
 do
 {
  c=ItA(a%n);
  if(c==ERR)
   return "ERR";
  res[l]=c;
  a/=n;
  l--;
 }
 while(a!=0); 
 f-=(int)f;
 if(f)
 {
  for(l=0;l<10;l++)
  {
   if(f==0)
   {
    aftdot[l]=0;
    break;
   }
   f*=n;
   aftdot[l]=ItA((char)f);
   f-=(char)f;
  }
 }
 else
 {
  dot=0;
 }
 aftdot[10]=0;
 if(res[0]==0)
 {
  res[0]='0';
  res[1]=0;
 }
 l=strlen(res)+15;
// m=(char*)calloc(l,1);
 m[0]=minus?'-':0;
// res=(char*)realloc(res,l);
// res=strcat(m,strcat(res,dot?strcat(d,aftdot):""));
 return strcat(m,strcat(res,dot?strcat(d,aftdot):""));
}

int main()
{
 int n1,n2,e;
 char s[255],s2[255],fool=0,inc=1;
 float a;
 do
 {
  if(fool)
   printf("Error! Base must be beetween 2 and 36.\n");
  printf(fool?"Try again: ":"Enter two bases: ");
  e=scanf("%d %d",&n1,&n2);
  if(e<2)
  {
   fool=1;
   scanf("%s",s); //dirty, but necessary...
   continue;
  }
  if(n1<2||n1>36||n2<2||n2>36)
   fool=1;
  else
   fool=0;
 }
 while(fool);
 do
 {
  if(fool)
  {
   printf("%s, try again!\n>",inc?"Incorrect number":"Error");
  }
  else
   printf("Enter a number in base %d: ",n1);
  scanf("%s",s);
  a=StFn(s,n1);
  if(a==ERR)
  {
   fool=1;
   continue;
  }
  else
   fool=0;
  strcpy(s2,FtSn(a,n2));
  if(strcmp(s2,"ERR")==0)
  {
   printf("Can't translate this number!\n");
   fool=1;
   inc=0;
  }
  else
  {
   printf("%s(%d) is %s(%d)\n",s,n1,s2,n2);
   fool=0;
  }
 }
 while(fool);
 return 0;
}