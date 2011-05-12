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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include "cbpm-index.h"
#include "cbpm-coloropts.h"

  signed char *audio;
unsigned long audiosize;
unsigned long audiorate=22050;  // perfect measure, 5512 is too lossy, 22050 takes too much time
unsigned long colorfreq=110;    // good ?
unsigned long colorperiod;
signed   long long *color;
signed  long long colorsadded=0;
long     int  bufsiz=32*1024;

signed char sign(signed char a, signed char b)
{
   if (a==b) return 0;
   if (a<b) return 1;
   return -1;
}

unsigned long findturn(unsigned long idx)
{
   signed long startsign;
   if (idx==audiosize) return idx;
   assert(idx<audiosize);
   startsign=sign(audio[idx],audio[idx+1]);
   if (startsign==0)
     while(idx<audiosize-1 && 
	   sign(audio[idx],audio[idx+1])==0) idx++;
   else
     while(idx<audiosize-1 &&
	   sign(audio[idx],audio[idx+1])!=-startsign) idx++;
   return idx;
}

void obtain_color_old()
{
   signed int times=0;
   signed long long i,t0,t1;
   colorperiod=audiorate/colorfreq;
   color=calloc(colorperiod,sizeof(signed long long));
   assert(color);
   /* the technique is simpel. We go voer the wave 
    * until we find a switch from going up to gfoing down. 
    * This point is marked T0. From then on we keep going 
    * until we find a switch from going down to going up.
    * We neglect this point completely, but call it T1.
    * We continue till we find the next turning point. 
    * Again we neglect it and call it T2. Now we start 
    * looking for our final opint T3. If we found it 
    * we rewrite the wave linearry between T0 and T3. 
    * The wave itself is maximized and supperposed 
    * over the colrowave. The wave is not normalised.
    * Normalisation is done by the number of same waves
    * encounterd.
    * 
    * This is done over the complete wave until we find
    * no more waves.
    */
   while (times<10 && (t0=findturn(0))!=audiosize)
     {
	signed long long v0,v1,si,cv0,cv1,c0,c;
	printf("colorsadded = %ld \n",colorsadded);
	while(1)
	  {
	     if ((t0+times*5)>=audiosize) 
	       t1=audiosize;
	     else
	       t1=findturn(t0+times*3)+1;
	     /* we store this piece of wave
	      * in the color-table stretched
	      * appropriately and subtracted with [t0]
	      */
	     if (t1>=audiosize) break;
	     assert(t0!=t1);
	     v0=audio[t0];
	     v1=audio[t1];
	     //  printf("%ld\n",(long)t1-(long)t0);
	     si=1;
	     c0=0;
	     cv0=0;
	     if (v1<v0) si=-1;
/*	     if (t1-t0<)
	       for(i=t0;i<=t1;i++)
		 {
		    signed long v=audio[i];
		    audio[i]=v0+(v1-v0)*(i-t0)/(t1-t0);
		 }
	     else
 */	       
	       {
		  for(c=0;c<colorperiod;c++)
		    {
		       signed long long v=(signed long long)audio[t0+c*(t1-t0)/(colorperiod-1)];
		       v=si*(v-(v0+c*(v1-v0)/(colorperiod-1)));
		       if (c==0) 
			 assert(v==0);
		       if (c==colorperiod-1) 
			 assert(v==0);
		       color[c]+=v;
		    }
/*		  for(i=t0;i<=t1;i++)
		    {
		       signed long long v=audio[i];
		       audio[i]=(signed char)(v0+(v1-v0)*(i-t0)/(t1-t0));
		    }
*/		  colorsadded++;
	       }
	     t0=t1;
	  };
	for(i=0;i<colorperiod;i++)
	  {
	     char d[500];
	     sprintf(d, "%d      %d",(int)i,(int)(((signed long long)256L)*color[i]/(signed long long)colorsadded));
	     index_addcomment(d);
	  }
	times++;
     };
   // dump result
}

void obtain_color()
{
   signed int times=0;
   signed long long i,t0,t1;
   colorperiod=audiorate/colorfreq;
   color=calloc(colorperiod,sizeof(signed long long));
   assert(color);
   /* since the previous techniques doesn't work
    * i'm trying this one. Instead of looking for ups
    * and downs we now start looking.
    * We take a number of random waves of a specific 
    * period and supperpose them on the colorwave for
    * the given period.
    * We take a piece of wave and find the best position
    * to place it over the color... This is maybe the best. 
    */
   while (times<10 && (t0=findturn(0))!=audiosize)
     {
	signed long long v0,v1,si,cv0,cv1,c0,c;
	printf("colorsadded = %ld \n",colorsadded);
	while(1)
	  {
	     if ((t0+times*5)>=audiosize) 
	       t1=audiosize;
	     else
	       t1=findturn(t0+times*3)+1;
	     /* we store this piece of wave
	      * in the color-table stretched
	      * appropriately and subtracted with [t0]
	      */
	     if (t1>=audiosize) break;
	     assert(t0!=t1);
	     v0=audio[t0];
	     v1=audio[t1];
	     //  printf("%ld\n",(long)t1-(long)t0);
	     si=1;
	     c0=0;
	     cv0=0;
	     if (v1<v0) si=-1;
/*	     if (t1-t0<)
	       for(i=t0;i<=t1;i++)
		 {
		    signed long v=audio[i];
		    audio[i]=v0+(v1-v0)*(i-t0)/(t1-t0);
		 }
	     else
 */	       
	       {
		  for(c=0;c<colorperiod;c++)
		    {
		       signed long long v=(signed long long)audio[t0+c*(t1-t0)/(colorperiod-1)];
		       v=si*(v-(v0+c*(v1-v0)/(colorperiod-1)));
		       if (c==0) 
			 assert(v==0);
		       if (c==colorperiod-1) 
			 assert(v==0);
		       color[c]+=v;
		    }
/*		  for(i=t0;i<=t1;i++)
		    {
		       signed long long v=audio[i];
		       audio[i]=(signed char)(v0+(v1-v0)*(i-t0)/(t1-t0));
		    }
*/		  colorsadded++;
	       }
	     t0=t1;
	  };
	for(i=0;i<colorperiod;i++)
	  {
	     char d[500];
	     sprintf(d, "%d      %d",(int)i,(int)(((signed long long)256L)*color[i]/(signed long long)colorsadded));
	     index_addcomment(d);
	  }
	times++;
     };
   // dump result
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
   printf("cBpmDj BPM Color Detector v%d.%d, Copyright (c) 2001 Werner Van Belle\n",MAJOR_VERSION,MINOR_VERSION);
   printf("This software is distributed under the GPL2 license. See copyright.txt for details\n");
   printf("See beatmixing.ps for details how to use the program.\n");
   printf("--------------------------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
   FILE * raw;
   char * tmp,d[500];
   int optct;
   signed short  buffer[bufsiz];
   long count,pos,i,startpercent=0,stoppercent=100;
   index_init();
   sprintf(d,"BpmDj v%d.%d",MAJOR_VERSION,MINOR_VERSION);
   index_version=strdup(d);
   optct=optionProcess(&cbpmcolorOptions,argc,argv);
   if (argc-optct!=1) USAGE(EXIT_FAILURE);
   if (!HAVE_OPT(NOCOPYRIGHT)) copyright();
   startpercent=OPT_VALUE_FROM;
   stoppercent=OPT_VALUE_TO;
   // de index_file is de naam van de file met het volledige pad binnen 
   // de music directory. Dit wil zeggen dat we de music er af moeten strippen
   index_file=strdup(argv[optct]);
   if (strstr(index_file,"music/")==index_file) index_file+=strlen("music/");
   else if (strstr(index_file,"./music/")==index_file) index_file+=strlen("./music/");
   else printf("Warning: song not in music/ or ./music/ direcotry\n");
   // de file die we wegschrijven is de songname met .idx ipv .mp3
   // deze data wordt in de huidige direcotry geschreven, tenzij anders 
   // opgegeven natuurlijk
   if (HAVE_OPT(WRITETO))
     index_readfrom=strdup(OPT_ARG(WRITETO));
   else
     {
	index_readfrom=strdup(basename(index_file));
	tmp=strstr(index_readfrom,".mp3");
	strcpy(tmp,".idx");
     }
   // de bpmcount_from en bpm_count_to fields moeten 
   // ingevuld worden
   index_bpmcount_from=startpercent;
   index_bpmcount_to=stoppercent;
   // now we have to create a raw mp3 file
   if (HAVE_OPT(VERBOSE))
     printf("Writing %s.raw\n",index_file);
   sprintf(d,"./glue-bpmraw \"%s\"\n",argv[optct]);
   system(d);
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
	     signed char reduw;
	     left=buffer[i];
	     right=buffer[i+1];
	     mean=left/256;
	     assert(abs(mean)<129);
	     if (pos+i/(2*(44100/audiorate))>=audiosize) break;
	     assert(pos+i/(2*(44100/audiorate))<audiosize);
	     audio[pos+i/(2*(44100/audiorate))]=(signed char)mean;
	  }
	pos+=count/(4*(44100/audiorate));
     }
   fclose(raw);
   if (HAVE_OPT(VERBOSE))
     printf("Retrieving wave type\n");
   sprintf(d," ");
   index_addcomment(d);
   obtain_color();
   index_write();
   // remove the old file
   sprintf(d,"%s.raw",basename(index_file));
   if (HAVE_OPT(VERBOSE))
     printf("removing file %s\n",d);
//   remove(d);
}
