/****
 BpmDj: Free Dj Tools
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


#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif /* HAVE_LIBGEN_H */

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif /* HAVE_ASSERT_H */

#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */

#include "cbpm-index.h"
#include "cbpm-periodopts.h"

#include "common.h"

unsigned char *audio;
unsigned long audiosize;
unsigned long audiorate=11025;  // perfect measure, 5512 is too lossy, 22050 takes too much time
unsigned long startbpm=120;
unsigned long stopbpm=160;

unsigned long startshift=0;
unsigned long stopshift=0;
long     int  bufsiz=32*1024;

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
   printf("cBpmDj BPM Counter v%d.%d, Copyright (c) 2001 Werner Van Belle\n",MAJOR_VERSION,MINOR_VERSION);
   printf("This software is distributed under the GPL2 license. See copyright.txt for details\n");
   printf("See beatmixing.ps for details how to use the program.\n");
   printf("--------------------------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
   FILE * raw;
   char * tmp,d[500];
   int optct, res2;
   signed short buffer[bufsiz];
   long count,pos,i,redux,startpercent=0,stoppercent=100;
   index_init();
   sprintf(d,"BpmDj v%d.%d",MAJOR_VERSION,MINOR_VERSION);
   index_version=strdup(d);
   optct=optionProcess(&cbpmcountOptions,argc,argv);
   if (argc-optct!=1) USAGE(EXIT_FAILURE);
   if (!HAVE_OPT(NOCOPYRIGHT)) copyright();
   startpercent=OPT_VALUE_FROM;
   stoppercent=OPT_VALUE_TO;
   // de index_file is de naam van de file met het volledige pad binnen 
   // de music directory. Dit wil zeggen dat we de music er af moeten strippen
   index_file=strdup(argv[optct]);
   if (strstr(index_file,"music/")==index_file) index_file+=strlen("music/");
   else if (strstr(index_file,"./music/")==index_file) index_file+=strlen("./music/");
   else printf("Warning: song not in music/ or ./music/ directory\n");
   // de file die we wegschrijven is de songname met .idx ipv .mp3
   // deze data wordt in de huidige direcotry geschreven, tenzij anders 
   // opgegeven natuurlijk
   if (HAVE_OPT(WRITETO))
     index_readfrom=strdup(OPT_ARG(WRITETO));
   else
     {
	index_readfrom=strdup(basename(index_file));
	tmp=strstr(index_readfrom,".mp3");
	if (!tmp) tmp=strstr(index_readfrom,".MP3");
	strcpy(tmp,".idx");
     }
   // de bpmcount_from en bpm_count_to fields moeten 
   // ingevuld worden
   index_bpmcount_from=startpercent;
   index_bpmcount_to=stoppercent;
   startbpm=OPT_VALUE_LOWESTBPM;
   stopbpm=OPT_VALUE_HIGHESTBPM;
   // first we look up the md5sum !
   if (HAVE_OPT(VERBOSE))
     printf("Obtaining md5sum for %s\n",argv[optct]);
   sprintf(d,"md5sum \"%s\" | awk '{printf $1}' >sum.tmp\n",argv[optct]);
   if (system(d)>256)
     {
	printf("Error: md5sum failed");
	exit(101);
     }
   else
     {
	FILE * kloink=fopen("sum.tmp","r");
	char s[40];
	int i=0;
	while(i<32)
	  {
	     int c = getc(kloink);
	     s[i]=c;
	     i++;
	  }
	s[32]=0;
	index_md5sum=strdup(s);
	fclose(kloink);
     }
   // now we have to create a raw mp3 file
   if (HAVE_OPT(VERBOSE))
     printf("md5sum = %s\nWriting %s.raw\n",index_md5sum,index_file);
   sprintf(d,"glue-bpmraw \"%s\"\n",argv[optct]);
   if (system(d)>256)
     {
	printf("Warning: glue-bpmraw failed, trying ./glue-bpmraw\n");
	sprintf(d,"./glue-bpmraw \"%s\"\n",argv[optct]);
	if (system(d)>256)
	  {
	     printf("Error: couldn't execute glue-bpmraw\n");
	     exit(100);
	  }
     }
   // the filename of the file to read is the basename 
   // suffixed with .raw
   sprintf(d,"%s.raw",basename(index_file));
   raw=fopen(d,"rb");
   if (!raw)
     {
	printf("Error: Unable to open %s\n",argv[optct]);
	exit(3);
     }
   // read complete file shrunken down into memory
   audiosize=fsize(raw);
   startpercent=(long)((long long)audiosize*(long long)startpercent/(long long)100);
   stoppercent=(long)((long long)audiosize*(long long)stoppercent/(long long)100);
   audiosize=stoppercent-startpercent;
   audiosize/=(4*(44100/audiorate));
   if (HAVE_OPT(VERBOSE)) 
     {
	printf("Reading from %ldk to %ldk ",startpercent/1024,stoppercent/1024);
	printf("(audiosize = %dk)\n",audiosize/1024);
	printf("Looking for tempo between %d and %d\n",startbpm,stopbpm);
     }
   audio=malloc(audiosize+1);
   if (!audio)
     {
	printf("Error: unable to allocate audio buffer\n");
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
	     if (pos+i/(2*(44100/audiorate))>=audiosize) break;
	     assert(pos+i/(2*(44100/audiorate))<audiosize);
	     audio[pos+i/(2*(44100/audiorate))]=(unsigned char)redux;
	  }
	pos+=count/(4*(44100/audiorate));
     }
   fclose(raw);
   if (HAVE_OPT(VERBOSE))
     printf("Finding best autocorrelation\n");
   sprintf(d," ");
   index_addcomment(d);
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
//	     printf(d,"# %d: %ld (%g BPM)\n",i,fout,
//		    4.0*(double)audiorate*60.0/(double)i);
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
//	     printf("# %d: %ld (%g BPM)\n",i,fout,
//		    4.0*(double)audiorate*60.0/(double)i);
	     if (minimumfout==0) maximumfout=minimumfout=fout;
	     if (fout<minimumfout) 
	       {
		  minimumfout=fout;
		  minimumfoutat=i;
	       }
	     if (fout>maximumfout) maximumfout=fout;
	  }
	// fill in period
	index_period=minimumfoutat;
	//	printf("# %d: %ld - %ld\n",minimumfoutat,minimumfout,maximumfout);
	for(i=startshift;i<stopshift;i++)
	  {
	     fout=foutat[i-startshift];
	     if (fout)
	       {
		  fout-=minimumfout;
		  // fout=(fout*100)/(maximumfout-minimumfout);
		  sprintf(d,"%g  %ld",4.0*(double)audiorate*60.0/(double)i,fout);
		  index_addcomment(d);
	       }
	  }
	sprintf(d,"%g",4.0*(double)audiorate*60.0/(double)minimumfoutat);
	index_tempo=strdup(d);
	printf("%s: %s\n",index_file,d);
     }
   index_write();
   // remove the old file
   sprintf(d,"%s.raw",basename(index_file));
   if (HAVE_OPT(VERBOSE))
     printf("removing file %s\n",d);
   remove(d);
}
