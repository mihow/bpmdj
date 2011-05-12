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
#include "cbpm-coloropts.h"

#include "common.h"

/*
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include "cbpm-index.h"
#include "cbpm-coloropts.h"
#include "fourier.h"
*/

  signed short int *audio;
unsigned long  int audiosize;
unsigned long  int audiorate=22050;  // perfect measure, 5512 is too lossy, 22050 takes too much time
unsigned       int fftsiz=2048;      // size of the fft
          long int bufsiz=32*1024;

void obtain_color()
{
   /* - het probleem met deze techniek is dat het een 
    *   gemiddelde neemt en de conpositie
    *   dus invloed heeft op de frequentieverdeling...
    * - een extra probleem is dat we een gemiddelde nodig 
    *   hebben om te weten wat denormale aard van het nummer
    *   is..
    */ 
   float inbuffer[fftsiz];
   float outbuffer[fftsiz];
   float outbuffer_i[fftsiz];
   float color[fftsiz];
   signed long long i=0,j;
   signed long long colorsadded=0;
   // empty color
   for(j=0;j<fftsiz;j++)
     color[j]=0;
   // scan data
   while (i<audiosize-fftsiz)
     {
	// fill buffer
	for(j=0;j<fftsiz;j++)
//	  {
//	     char d[500];
	     inbuffer[j]=(float)audio[i+j];
//	     sprintf(d, "%d      %g",(int)i,(int)(((signed long long)256L)*color[i]/(signed long long)colorsadded));
//	     index_addcomment(d);
//	  }
	// fft buffer
	fft_float(fftsiz,0,inbuffer,NULL,outbuffer,outbuffer_i);
	// read outbuffer
	for(j=0;j<fftsiz;j++)
	  color[j]+=
	  sqrt(
	       (double)((outbuffer[j]+outbuffer_i[j]))*
	       (double)((outbuffer[j]+outbuffer_i[j]))
	       );
	i+=fftsiz;
	colorsadded++;
//	break;
     };
   printf("colorsadded = %d\n",colorsadded);
   for(j=0;j<fftsiz/2;j++)
     color[j]+=color[fftsiz-j];
   for(j=0;j<fftsiz/2;j++)
     color[j]/=(float)colorsadded;
   for(i=0;i<fftsiz/2;i++)
     {
	char d[500];
	sprintf(d, "%d      %g",(int)i,log(fabsf(color[i])));
	index_addcomment(d);
     }
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
   audio=calloc(audiosize+1,sizeof(signed short int));
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
	     signed long int mean;
	     mean=buffer[i];
	     if (pos+i/(2*(44100/audiorate))>=audiosize) break;
	     assert(pos+i/(2*(44100/audiorate))<audiosize);
	     audio[pos+i/(2*(44100/audiorate))]=mean;
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
