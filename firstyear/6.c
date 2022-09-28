#include <stdio.h>
#include <string.h>
int main(int argc,char *argv[])
{
	char filename[256],freq[256];
	unsigned char c;
	int i;
	strcpy(filename,argc>1?argv[1]:"input.txt");
	freopen(filename,"r",stdin);
	memset(freq,0,256);
	while(scanf("%c",&c)!=EOF)
	{
		freq[c]++;
	}
	for(i=0;i<256;i++)
		if(freq[i])
			if(i!='\n')
				printf("Character '%c' accured %d times.\n",i,freq[i]);
			else
				printf("Character '\\n' accured %d times.\n",freq[i]);
	return 0;
}