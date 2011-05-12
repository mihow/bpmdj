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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <assert.h>
#include <math.h>
#include "cbpm-index.h"
#include "version.h"
#include "common.h"

unsigned char *audio;
unsigned long audiosize;
unsigned long audiorate=11025;  // perfect measure, 5512 is too lossy, 22050 takes too much time
unsigned long startbpm=120;
unsigned long stopbpm=160;

unsigned long startshift=0;
unsigned long stopshift=0;
long     int  bufsiz=32*1024;

static   int  opt_verbose=0;
static   int  opt_create=0;
static   int  arg_low=120;
static   int  arg_high=160;
static   int  arg_from=0;
static   int  arg_to=100;
static   int  opt_write=0;
static char * arg_write;
static char * argument;

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
   printf("cBpmDj BPM Counter v%s, Copyright (c) 2001 Werner Van Belle\n",VERSION);
   printf("This software is distributed under the GPL2 license. See copyright.txt for details\n");
   printf("See beatmixing.ps for details how to use the program.\n");
   printf("--------------------------------------------------------------------\n");
}

void options_failure(char* err)
{
   // print options
   printf("Usage:  cbpm-count <options> argument\n\n"
	  "   -v       --verbose        be verbose\n"
	  "   -t nbr   --to nbr         read to % (default = 100)\n"
	  "   -f nbr   --from nbr       read from % (default = 0)\n"
	  "   -l nbr   --low nbr        lowest bpm to look for (default = 120)\n"
	  "   -h nbr   --high nbr       highest bpm to look for (default = 160)\n"
	  "   -w arg   --write arg      write .idx file to ...\n"
	  "   -c       --create         do not actually measure BPM, simple write an index file\n"
	  "   argument                  the .mp3 file to find tempo for\n\n%s\n\n",err);
   exit(1);
}

void process_options(int argc, char* argv[])
{
   // run trough all the arguments
   int i = 1 ;
   for(i = 1 ; i < argc ; i ++)
     {
	if (argv[i][0]=='-')
	  {
	     char* arg;
	     // check long opt ?
	     if (argv[i][1]=='-')
	       arg=argv[i]+2;
	     else if (argv[i][1]==0 || argv[i][2]!=0)
	       options_failure("option neither short or long");
	     else arg=argv[i]+1;
	     // check value
	     if (strcmp(arg,"verbose")==0 ||
		 strcmp(arg,"v")==0)
	       opt_verbose=1;
	     if (strcmp(arg,"create")==0 ||
		 strcmp(arg,"c")==0)
	       opt_create=1;
	     else if (strcmp(arg,"from")==0 ||
		      strcmp(arg,"f")==0)
	       {
		  if (++i>=argc) 
		    options_failure("from argument scanning error");
		  arg_from=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"to")==0 ||
		      strcmp(arg,"t")==0)
	       {
		  if (++i>=argc) 
		    options_failure("to argument scanning error");
		  arg_to=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"low")==0 ||
		      strcmp(arg,"l")==0)
	       {
		  if (++i>=argc) 
		    options_failure("low argument scanning error");
		  arg_low=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"high")==0 ||
		      strcmp(arg,"h")==0)
	       {
		  if (++i>=argc)
		    options_failure("high argument scanning error");
		  arg_high=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"write")==0 ||
		      strcmp(arg,"w")==0)
	       {
		  opt_write=1;
		  if (++i>=argc)
		    options_failure("write argument scanning error");
		  arg_write=argv[i];
	       }
	  }
	else
	  argument = argv[i];
     }
   if (argument==NULL)
     {
	options_failure("requires at least one argument");
     }
   
}

int main(int argc, char *argv[])
{
   FILE * raw;
   char * tmp,d[500];
   int res2;
   signed short buffer[bufsiz];
   long count,pos,i,redux,startpercent=0,stoppercent=100;
   index_init();
   sprintf(d,"BpmDj v%s",VERSION);
   index_version=strdup(d);
   process_options(argc,argv);
   copyright();
   startpercent=arg_from;
   stoppercent=arg_to;
   // de index_file is de naam van de file met het volledige pad binnen 
   // de music directory. Dit wil zeggen dat we de music er af moeten strippen
   index_file=strdup(argument);
   if (strstr(index_file,"music/")==index_file) index_file+=strlen("music/");
   else if (strstr(index_file,"./music/")==index_file) index_file+=strlen("./music/");
   else printf("Warning: song not in music/ or ./music/ directory\n");
   // de file die we wegschrijven is de songname met .idx ipv .mp3
   // deze data wordt in de huidige direcotry geschreven, tenzij anders 
   // opgegeven natuurlijk
   if (opt_write)
     index_readfrom=strdup(arg_write);
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
   startbpm=arg_low;
   stopbpm=arg_high;
   // first we look up the md5sum !
   if (opt_verbose)
     printf("Obtaining md5sum for %s\n",argument);
   sprintf(d,"md5sum \"%s\" | awk '{printf $1}' >sum.tmp\n",argument);
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
   if (opt_verbose)
     printf("md5sum = %s\n",index_md5sum);
   // now we have to create a raw mp3 file
   if (!opt_create)
     {
	if (opt_verbose)
	  printf("Writing %s.raw\n",index_file);
	sprintf(d,"glue-bpmraw \"%s\"\n",argument);
	if (system(d)>256)
	  {
	     printf("Warning: glue-bpmraw failed, trying ./glue-bpmraw\n");
	     sprintf(d,"./glue-bpmraw \"%s\"\n",argument);
	     if (system(d)>256)
	       {
		  printf("Error: couldn't execute glue-bpmraw\n");
		  exit(100);
	       }
	  }
     }
   else if (opt_verbose)
     {
	printf("Not writing .raw file\n");
     }
   // the filename of the file to read is the basename 
   // suffixed with .raw
   if (!opt_create)
     {
	sprintf(d,"%s.raw",basename(index_file));
	raw=fopen(d,"rb");
	if (!raw)
	  {
	     printf("Error: Unable to open %s\n",argument);
	     exit(3);
	  }
	// read complete file shrunken down into memory
	audiosize=fsize(raw);
	startpercent=(long)((long long)audiosize*(long long)startpercent/(long long)100);
	startpercent-=startpercent%4;
	stoppercent=(long)((long long)audiosize*(long long)stoppercent/(long long)100);
	stoppercent-=stoppercent%4;
	audiosize=stoppercent-startpercent;
	audiosize/=(4*(44100/audiorate));
	if (opt_verbose)
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
	     count=fread(buffer,sizeof(signed short),bufsiz,raw);
	     for (i=0;i<count;i+=2*(44100/audiorate))
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
	     pos+=count/(2*(44100/audiorate));
	  }
	fclose(raw);
	if (opt_verbose)
	  printf("Finding best autocorrelation\n");
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
	     sprintf(d,"%g",4.0*(double)audiorate*60.0/(double)minimumfoutat);
	     index_tempo=strdup(d);
	     printf("%s: %s\n",index_file,d);
	  }
     }
   else // opt_create
     {
	printf("Not measuring tempo, simply writing inf\n");
	index_tempo="inf";
	index_period=0;
     }
   index_write();
   // remove the old file
   if (!opt_create)
     {
	sprintf(d,"%s.raw",basename(index_file));
	if (opt_verbose)
	  printf("Removing file %s\n",d);
	remove(d);
     }
}
