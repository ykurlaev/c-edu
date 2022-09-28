#include <stdio.h>

unsigned int months[13]={0,31,0,31,30,31,30,31,31,30,31,30,31};

long int DaysTo(unsigned int d,unsigned int m,unsigned int y)
{
 long int res;
 unsigned int i;
 y--;
 res=365*y+y/4-y/100+y/400;
 y++;
 for(i=1;i<=m;i++)
 {
  if(i!=2)
  {
   res+=months[i];
  }
  else
  {
   res+=(!(y%4)&&(y%100))||!(y%400)?29:28;
  }
 }
 res+=d;
 return res;
}

int main()
{
 unsigned int fd,fm,fy,sd,sm,sy;
 char fool=0,s[255];
 int e;
 do
 {
  printf("Enter two dates (in \"dd mm yyyy\" format): \n");
  e=scanf("%d %d %d %d %d %d",&fd,&fm,&fy,&sd,&sm,&sy);
  fool=!((fd>0)&&(((fm!=2)&&(fd<=months[fm]))||((fm==2)&&(fd<=((!(fy%4)&&(fy%100))||!(fy%400)?29:28))))&&(fm>0)&&(fm<13)&&
   (sd>0)&&(((sm!=2)&&(sd<=months[sm]))||((sm==2)&&(sd<=(((!(sy%4)&&(sy%100))||!(sy%400)?29:28)))))&&(sm>0)&&(sm<13)&&(e==6));
  if(fool)
  {
   printf("Incorrect input!\n");
   if(e!=6)
    scanf("%s",s);
  }
  else
   printf("%d days between %d.%d.%d and %d.%d.%d\n",DaysTo(sd,sm,sy)-DaysTo(fd,fm,fy),fd,fm,fy,sd,sm,sy);
 }
 while(fool);
 return 0;
}
