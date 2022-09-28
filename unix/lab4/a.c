#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	int i;
#ifndef NS
	sleep(10);
#endif
	printf("Start\n");
#ifndef NS
	sleep(5);
#endif
	char *a=malloc(1024);
	printf("Malloced KiB: %p\n",a);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<1024;i++)
		a[i]=0;
	printf("Used KiB\n");
#ifndef NS
	sleep(5);
#endif
	char *b=malloc(128*1024-1);
	printf("Malloced 128 KiB - 1 byte: %p\n",b);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<128*1024-1;i++)
		b[i]=0;
	printf("Used 128 KiB - 1 byte\n");
#ifndef NS
	sleep(5);
#endif
	char *bb=malloc(128*1024-1);
	printf("Malloced another 128 KiB - 1 byte: %p\n",bb);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<128*1024-1;i++)
		bb[i]=0;
	printf("Used another 128 KiB - 1 byte\n");
#ifndef NS
	sleep(5);
#endif
	char *aa=malloc(1024);
	printf("Malloced another KiB: %p\n",aa);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<1024;i++)
		aa[i]=0;
	printf("Used KiB\n");
#ifndef NS
	sleep(5);
#endif
	char *bbb=malloc(128*1024-1);
	printf("Malloced yet another 128 KiB - 1 byte: %p\n",bbb);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<128*1024-1;i++)
		bbb[i]=0;
	printf("Used yet another 128 KiB - 1 byte\n");
#ifndef NS
	sleep(5);
#endif
	char *c=malloc(128*1024);
	printf("Malloced 128 KiB: %p\n",c);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<128*1024;i++)
		c[i]=0;
	printf("Used 128 KiB\n");
#ifndef NS
	sleep(5);
#endif
	char *d=malloc(1024*1024);
	printf("Malloced MiB: %p\n",d);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<1024*1024;i++)
		d[i]=0;
	printf("Used MiB\n");
#ifndef NG
#ifndef NS
	sleep(5);
#endif
	char *e=malloc(1024*1024*1024);
	printf("Malloced GiB: %p\n",e);
#ifndef NS
	sleep(5);
#endif
	for(i=0;i<1024*1024*1024;i++)
		e[i]=0;
	printf("Used Gib\n");
#endif
#ifndef NS
	sleep(5);
#endif
	free(a);
	printf("Freed KiB\n");
#ifndef NG
#ifndef NS
	sleep(5);
#endif
	free(e);
	printf("Freed GiB\n");
#endif
#ifndef NS
	sleep(5);
#endif
	free(d);
	printf("Freed MiB\n");
#ifndef NS
	sleep(5);
#endif
	free(b);
	printf("Freed 128 - 1 KiB\n");
#ifndef NS
	sleep(5);
#endif
	free(bb);
	printf("Freed another 128 - 1 KiB\n");
#ifndef NS
	sleep(5);
#endif
	free(bbb);
	printf("Freed yet another 128 - 1 KiB\n");
#ifndef NS
	sleep(5);
#endif
	free(c);
	printf("Freed 128 KiB byte\n");
#ifndef NS
	sleep(5);
#endif
	free(aa);
	printf("Freed another KiB\n");
#ifndef NS
	sleep(5);
#endif
	return 0;
}
