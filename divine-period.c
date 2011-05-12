/****
 Divine Intervention: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

unsigned char *audio;
unsigned long audiosize;
unsigned long audiorate=11025;  // perfect measure, 5512 is too lossy, 22050 takes too much time
unsigned long startbpm=120;
unsigned long stopbpm=160;


unsigned long startshift=0;
unsigned long stopshift=0;
long int bufsiz=32*1024;

void drawbar(unsigned char val)
{
   signed long res;
   res=((long)val)*80/256;
   while(res>0)
     {
	printf(" ");
	res--;
     }
   printf("|\n");
}

unsigned long phasefit(long i)
{
   long c,d;
   unsigned long mismatch=0;
   unsigned long prev=mismatch;
   for(c=i;c<audiosize;c++)
     {
	d=abs((long)audio[c]-(long)audio[c-i]);
	prev=mismatch;
	mismatch+=d;
	assert(mismatch>=prev);
     }
   return mismatch;
}

long fsize(FILE * f)
{
   long answer;
   fseek(f,0,SEEK_END);
   answer=ftell(f);
   fseek(f,0,SEEK_SET);
   return answer;
}

void copyright()
{
   printf("# Divine Intervention Player v0.5, Copyright (c) 2001 Werner Van Belle\n");
   printf("# This software is distributed under the GPL2 license. See Gpl2.txt for details\n");
   printf("# See BeatMixing.ps for details how to use the program.\n");
   printf("# --------------------------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
   FILE *raw;
   signed short buffer[bufsiz];
   long count,pos,i,redux,startpercent=0,stoppercent=100;
   copyright();
   if (argc<2)
     {
	printf("Expecting 1, 2 or 3 argument(s)\n");
	exit(1);
     }
   raw=fopen(argv[1],"rb");
   if (argc>=3)
     {
	startpercent=atoi(argv[2]);
	printf("# Starting from position %ld\n",startpercent);
     }
   if (argc>=4)
     {
	stoppercent=atoi(argv[3]);
	printf("# Stopping at position %ld\n",stoppercent);
     }
   if (!raw) 
     {
	printf("unable to open %s\n",argv[1]);
	exit(3);
     }
   // read complete file shrunken down into memory
   audiosize=fsize(raw);
   startpercent=(long)((long long)audiosize*(long long)startpercent/(long long)100);
   stoppercent=(long)((long long)audiosize*(long long)stoppercent/(long long)100);
   audiosize=stoppercent-startpercent;
   printf("# Reading from %ldk to %ldk\n",startpercent/1024,stoppercent/1024);
   audiosize/=(4*(44100/audiorate));
   printf("# Audiosize = %dk\n",audiosize/1024);
   audio=malloc(audiosize+1);
   if (!audio)
     {
	printf("unable to allocate audio buffer\n");
	exit(4);
     }
   pos=0;
   fseek(raw,startpercent,SEEK_SET);
   while(pos<audiosize)
     {
	count=fread(buffer,1,bufsiz,raw);
	for (i=0;i<count/2;i+=2*(44100/audiorate))
	  {
	     signed long int left, right,mean;
	     left=abs(buffer[i]);
	     right=abs(buffer[i+1]);
	     mean=(left+right)/2;
	     redux=abs(mean)/128;
//	     drawbar(redux);
	     if (pos+i/(2*(44100/audiorate))>=audiosize) break;
	     assert(pos+i/(2*(44100/audiorate))<audiosize);
	     audio[pos+i/(2*(44100/audiorate))]=(unsigned char)redux;
	  }
	pos+=count/(4*(44100/audiorate));
     }
   fclose(raw);
   stopshift=audiorate*60*4/startbpm;
   startshift=audiorate*60*4/stopbpm;
     {
	unsigned long foutat[stopshift-startshift];
	unsigned long fout, minimumfout=0, maximumfout,minimumfoutat,left,right;
	memset(&foutat,0,sizeof(foutat));
	for(i=startshift;i<stopshift;i+=50)
	  {
	     fout=phasefit(i);
	     foutat[i-startshift]=fout;
	     printf("# %d: %ld (%g BPM)\n",i,fout,
		    4.0*(double)audiorate*60.0/(double)i);
	     if (minimumfout==0) maximumfout=minimumfout=fout;
	     if (fout<minimumfout) 
	       {
		  minimumfout=fout;
		  minimumfoutat=i;
	       }
	     if (fout>maximumfout) maximumfout=fout;
	  }
	left=minimumfoutat-100;
	right=minimumfoutat+100;
	if (left<startshift) left=startshift;
	if (right>stopshift) right=stopshift;
	for(i=left;i<right;i++)
	  {
	     fout=phasefit(i);
	     foutat[i-startshift]=fout;
	     printf("# %d: %ld (%g BPM)\n",i,fout,
		    4.0*(double)audiorate*60.0/(double)i);
	     if (minimumfout==0) maximumfout=minimumfout=fout;
	     if (fout<minimumfout) 
	       {
		  minimumfout=fout;
		  minimumfoutat=i;
	       }
	     if (fout>maximumfout) maximumfout=fout;
	  }
	printf("# %d: %ld - %ld\n",minimumfoutat,minimumfout,maximumfout);
	for(i=startshift;i<stopshift;i++)
	  {
	     fout=foutat[i-startshift];
	     if (fout)
	       {
		  fout-=minimumfout;
		  // fout=(fout*100)/(maximumfout-minimumfout);
		  printf("%g  %ld \n",4.0*(double)audiorate*60.0/(double)i,fout);
	       }
	  }
	printf("# TEMPO: %g\n",4.0*(double)audiorate*60.0/(double)minimumfoutat);
     }
}
