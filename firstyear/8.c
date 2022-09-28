#include <stdio.h>
#define type long long
int main()
{
        type N,oldN,newN=0;
        unsigned int K,i;
        scanf("%lld%d",&N,&K);
        oldN=N;
        for(i=0;i<sizeof(N)*8;i++)
        {
                printf("%lld",((unsigned type)N&((unsigned type)1<<(sizeof(N)*8-1)))>>(sizeof(N)*8-1));
                N<<=1;
        }
	N=oldN;
        for(i=0;i<sizeof(N)*8;i++)
        {
                newN<<=1;
                newN|=N&1;
                N>>=1;
        }
	printf("\n%lld ",newN);
        for(i=0;i<sizeof(N)*8;i++)
        {
                printf("%lld",((unsigned type)newN&((unsigned type)1<<(sizeof(N)*8-1)))>>(sizeof(N)*8-1));
                newN<<=1;
        }
	N=oldN>>K;
	printf("\n%lld ",N);
        for(i=0;i<sizeof(N)*8;i++)
        {
                printf("%lld",((unsigned type)N&((unsigned type)1<<(sizeof(N)*8-1)))>>(sizeof(N)*8-1));
                N<<=1;
        }	
	N=oldN<<K;
	printf("\n%lld ",N);
        for(i=0;i<sizeof(N)*8;i++)
        {
                printf("%lld",((unsigned type)N&((unsigned type)1<<(sizeof(N)*8-1)))>>(sizeof(N)*8-1));
                N<<=1;
        }	
        printf("\n");
        return 0;
}
