#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

main()
{
  time_t now;
  struct tm *tm_now;
  time(&now);
  putenv("TZ=PST8PDT");
  printf("ctime: %s", ctime(&now));
  tm_now=localtime(&now);
  printf("localtime: %02d/%02d/%02d %02d:%02d %s\n",
         tm_now->tm_mon + 1,
         tm_now->tm_mday,
         tm_now->tm_year % 100,
         tm_now->tm_hour,
         tm_now->tm_min,
         tzname[tm_now->tm_isdst]);
  return 0;
}