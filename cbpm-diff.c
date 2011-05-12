#include <stdlib.h>
#include <stdio.h>
#include "cbpm-index.h"
#define fftsiz (1024)
static float color1[fftsiz];
static float color2[fftsiz];

void normalise(float color[])
{
   float sum=0;
   int i;
   for(i=0;i<fftsiz;i++)
     sum+=color[i];
   sum/=fftsiz;
   for(i=0;i<fftsiz;i++)
     color[i]-=sum;
}

int main(int argc, char* argv[])
{
   signed i;
   index_init();
   index_read(argv[1]);
   for(i=0;i<index_nextcomment;i++)
     {
	int idx;
	float flt;
	sscanf(index_comments[i],"%d      %g\n",&idx,&flt);
	printf("%d; %g\n",idx,flt);
	if (idx>=0 && idx<1024)
	  color1[idx]=flt;
     }
   index_init();
   index_read(argv[2]);
   for(i=0;i<index_nextcomment;i++)
     {
	int idx;
	float flt;
	sscanf(index_comments[i],"%d      %g\n",&idx,&flt);
	printf("%d; %g\n",idx,flt);
	if (idx>=0 && idx<1024)
	  color2[idx]=flt;
     }
   index_init();
   index_readfrom=strdup("diff.idx");
   normalise(color1);
   normalise(color2);
   for(i=0;i<fftsiz;i++)
     {
	char d[500];
	sprintf(d,"%d      %g",i,color1[i]-color2[i]);
	index_addcomment(d);
     }
   index_write();
}
