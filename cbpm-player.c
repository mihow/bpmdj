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

/*-------------------------------------------
 *         Todo
 *-------------------------------------------
 * - playing < zero check when going forward/backward
 * - pause/play
/*-------------------------------------------
 *         Headers
 *-------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <termios.h>
#include <mntent.h>
#include <getopt.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <linux/soundcard.h>
#include <math.h>
#include "cbpm-playeropts.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define            WAVRATE  (22050)
#define            inbufsiz (32*1024)
              char *dsp=NULL;
              long dmabufsiz=0;
  signed long long targetperiod;
  signed long long currentperiod;
  signed long long normalperiod;
  signed long long latency;
  signed long long y,x=0;
               int start=1;
               int stop=0;
       static char *songname=NULL;

/*-------------------------------------------
 *         File output oprations
 *-------------------------------------------*/
int outfile;
static clock_t starttick;
void startdmaplaying()
{
   starttick=times(NULL);
}

signed long long guessdmaplaycount()
{
   long long ticks=(long long)times(NULL)-(long long)starttick;
   long long answer=ticks*(long long)WAVRATE/(long long)CLK_TCK;
   assert(answer>=0);
   return answer;
}

signed long long checklatency()
{
   
}

void open_outfile()
{
   int p;
   double latency;
   audio_buf_info ab;
   outfile=open(dsp,O_WRONLY,0);
   if (!outfile)
     {
	printf("unable to open /dev/dsp\n");
	exit(2);
     }
   p=16;
   ioctl(outfile,SOUND_PCM_WRITE_BITS,&p);
   p=2;
   ioctl(outfile,SOUND_PCM_WRITE_CHANNELS,&p);
   p=WAVRATE;
   ioctl(outfile,SOUND_PCM_WRITE_RATE,&p);
   ioctl(outfile,SNDCTL_DSP_GETOSPACE,&ab);
   dmabufsiz=ab.bytes/4;
   if (HAVE_OPT(DEBUGLATENCY))
     {
	latency=(double)ab.bytes;
	latency/=4.0;
	latency/=(double)WAVRATE;
	latency*=1000.0;
	printf("Dsp device has %d bytes: latency = %g ms\n",ab.bytes,latency);
	startdmaplaying();
	sleep(1);
	printf("Clock gives %d ticks (should be around %d)\n",(long)guessdmaplaycount(),WAVRATE);
     }
}

void flushdsp()
{
   ioctl(outfile,SNDCTL_DSP_SYNC);
}

signed long long dmaplaycount()
{
   count_info cnt;
   ioctl(outfile,SNDCTL_DSP_GETOPTR,&cnt);
   return cnt.bytes/4;
}

int dmabufferpos()
{
   count_info cnt;
   ioctl(outfile,SNDCTL_DSP_GETOPTR,&cnt);
   /* printf("bufferpos = %d\n",cnt.ptr/4);
    */
   return cnt.ptr/2;
}

/*-------------------------------------------
 *         File input oprations
 *-------------------------------------------*/
unsigned long fsize(FILE* wtf)
{
   unsigned long res;
   fseek(wtf,0,SEEK_END);
   res=ftell(wtf);
   fseek(wtf,0,SEEK_SET);
   return res;
}

unsigned long bufferin[inbufsiz];
FILE*         infile;
unsigned long bufferinpos=-inbufsiz;

void open_infile(char* fname)
{
   infile=fopen(fname,"rb");
   if (!infile) 
     {
	printf("unable to open %s\n",fname);
	exit(3);
     }
}

unsigned long readat(unsigned long pos)
{
   if (pos-bufferinpos>=inbufsiz)
     {
	bufferinpos=pos;
//	printf("Seeking to position %d k\n",pos*4/1024);
	fseek(infile,pos*4,SEEK_SET);
	fread(bufferin,4,inbufsiz,infile);
     }
   return bufferin[pos-bufferinpos];
}

unsigned long readmax()
{
   return fsize(infile)/4;
}

/*-------------------------------------------
 *         Terminal routines  
 *-------------------------------------------*/
void useraction(int neglect);
void rl_ttyset (int Reset)
{
   long i;
   static struct termios old;
   struct termios new;
   if (Reset == 0)
     {(void) tcgetattr (0, &old);
	new = old;
	new.c_lflag &= ~(ECHO | ICANON);
	new.c_iflag &= ~(ISTRIP | INPCK);
	(void) tcsetattr (0, TCSANOW, &new);}
   else (void) tcsetattr (0, TCSANOW, &old);
   fcntl(0,F_SETFL,O_ASYNC | O_NONBLOCK);
   i=getpid();
   fcntl(0,F_SETOWN,i);
   signal(SIGIO, useraction);
   signal(SIGURG, useraction);
}

void printbar(signed short int val)
{
   signed long res;
   res=((long)val)*40/32768;
   res+=40;
   while(res>0)
     {
	printf(" ");
	res--;
     }
   printf("|\n");
}

/*-------------------------------------------
 *         Program logic
 *-------------------------------------------*/
void copyright();
void help();

void changetempo(signed long long period)
{
   /* change tempo to period
    * - the current x should remain the same
    * x = y * normalperiod / currentperiod
    * y = x * currentperiod / normalperiod
    */
//   printf("Tempo change to %d\n",period);
   currentperiod = period;
   y = x * currentperiod / normalperiod;
}

void printpos(char* text)
{
   unsigned long m=readmax();
   printf("%s: %d % (%d/%d/%d)\n",text,(long)(y*100/m),(long)dmaplaycount()/1024,(long)y/1024,(long)m/1024);
}

void useraction(int neglect)
{
   static unsigned long marktime=0;
   static unsigned long long markat=0;
   static signed long long tempo_fade=0; // 0 = targetperiod; 10 = normalperiod
   int c,nr;
   c=0;
   nr=read(0,&c,1);
   if (c==0) return;
   if (c=='q') 
     stop=1;
   else if (c=='+')
     y-=currentperiod/(4*32);
   else if (c=='-')
     y+=currentperiod/(4*32);
   else if (c=='*')
     y+=currentperiod/(8);
   else if (c=='\n')
     y+=currentperiod/(4);
   else if (c=='1')
     {
	y-=currentperiod;
	printpos("-1 Measure");
     }
   else if (c=='2')
     {
	y+=currentperiod;
	printpos("+1 Measure");
     }
   else if (c=='4')
     {
	y-=currentperiod*4;
	printpos("-4 Measure");
     }
   else if (c=='5')
     {
	y+=currentperiod*4;
	printpos("+4 Measure");
     }
   else if (c=='7')
     {
	y-=currentperiod*8;
	printpos("-8 Measure");
     }
   else if (c=='8')
     {
	y+=currentperiod*8;
	printpos("+8 Measure");
     }
   else if (c=='%')
     {
	printf("Song: %s\n",songname);
	printpos("Current position: ");
     }
   else if (c=='[')
     {
	markat=dmaplaycount();
	printf("Marking master beat\n");
     }
   else if (c==']')
     {
	unsigned long long fixit;
	unsigned long long measures;
	markat=dmaplaycount()-markat;
	fixit=markat;
	printf("Fixing slave beat\n");
	y-=fixit;
     }
   else if (c=='/')
     {
	printf("Marking target beat;\n");
	markat=y;
     }
   else if (c=='9')
     {
	signed long long fixit;
	unsigned long long gotopos=markat;
	fixit=(y%currentperiod)-(gotopos%currentperiod);
	printf("Started at target beat (fixit = %d)\n",(signed long)fixit);
	y=gotopos+fixit;
     }
   else if (c=='6')
     {
	signed long long fixit;
	unsigned long long gotopos=markat-currentperiod*8;
	fixit=(y%currentperiod)-(gotopos%currentperiod);
	printf("Started at target beat -8 measures (fixit = %d)\n",(signed long)fixit);
	y=gotopos+fixit;
     }
   else if (c=='3')
     {
	signed long long fixit;
	unsigned long long gotopos=markat-currentperiod*16;
	fixit=(y%currentperiod)-(gotopos%currentperiod);
	printf("Started at target beat -16 measures (fixit = %d)\n",(signed long)fixit);
	y=gotopos+fixit;
     }
   else if (c=='>' || c=='0')
     {
	if (tempo_fade<10) tempo_fade=tempo_fade+1;
	printf("Tempo fade %d\n",tempo_fade);
	
	changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10);
     }
   else if (c=='<')
     {
	if (tempo_fade>0) tempo_fade=tempo_fade-1;
	printf("Tempo fade %d\n",tempo_fade);
	changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10);
     }
   else if (c=='.')
     {
	changetempo(normalperiod);
	printf("Tempo normal\n");
     }
   else if (c==',')
     {
	changetempo(targetperiod);
	printf("Tempo target\n");
     }
   else if (c=='l')
     {
	changetempo(currentperiod/1.05946);
     }
   else if (c=='k')
     {
	changetempo(currentperiod*1.05946);
     }
   else if (c=='t')
     {
	if (marktime)
	  printf("Seconds elapsed sinds mark %d\n",time(NULL)-marktime);
	else
	  {
	     printf("Time marked\n");
	     marktime=time(NULL);
	  }
     }
   else if (c=='{')
     {
	printf("Back to the beginning, normal tempo\n");
	y=0;
	x=0;
	changetempo(normalperiod);
     }
   else if (c=='c')
     {
	copyright();
     }
   else if (c=='?' || c=='h')
     {
	help();
     }
   if (y<0)
     {
	printf("y underflow, setting to zero\n");
	y=0;
     }
   useraction(neglect);
}

void fillbuffer()
{
   signed long long latencycheck;
   signed long long selfplaycount=-latency;
   unsigned long value[1];
   startdmaplaying();
   if (start) start=0;
   while(!stop)
     {
	x=y*normalperiod/currentperiod;
	value[0]=readat(x);
	y++;
	while(selfplaycount>guessdmaplaycount()) ;
	selfplaycount++;
	write(outfile,value,4);
     }
   if (HAVE_OPT(DEBUGLATENCY))
     {
	latencycheck=guessdmaplaycount();
	flushdsp();
	printf("Actual playing latency = %d ms\n",(long)((guessdmaplaycount()-latencycheck)*(long long)1000/(long long)WAVRATE));
     }
}

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
void line()
{
   printf("--------------------------------------------------------------------\n");
}

void copyright()
{
   printf("cBpmDj Player v%d.%d, Copyright (c) 2001 Werner Van Belle\n",MAJOR_VERSION,MINOR_VERSION);
   printf("This software is distributed under the GPL2 license. See Gpl2.txt for\n");
   printf("details. Press 'h' for Help. See BeatMixing.ps for details\n");
   line();
}

void help()
{
   line();
   printf("1) Movement\n"
	  "     1 : -1M                   2 : +1M\n"
	  "     4 : -4M                   5 : +4M\n"
	  "     7 : -8M                   8 : +8M\n"
	  "     + : -1/32N                - : +1/32N    \n"
	  "     * : -1/2B           <ENTER> : +1B \n"
	  "2) Sliding\n"
	  "     [ : mark 'startbeat' in monitorred song\n"
	  "     ] : mark 'startbeat' in playing song + set monitorred song to difference with 'startbeat'\n"
	  "     / : Set song position mark\n"
	  "     9 : Go to position mark\n"
	  "     6 : Go to position mark -8M\n"
	  "     3 : Go to position mark -16M\n"
	  "3) Tempo: change it to normal\n"
	  "     l : Pitch += 1/2   \n"       
	  "     k : Pitch -= 1/2\n"
	  "     , : Tempo = target tempo  \n"
	  "     . : Tempo = normal tempo \n"
	  "     0 or > : Tempostep to normal tempo\n"
	  "     < : Tempostep to target tempo\n"
	  "     { : Beginning of song at normal tempo\n"
	  "4) Misc\n"
	  "     t : first press: marks time\n"
	  "       : second press: shows time difference\n"
	  "     % : current position and name\n"
	  "     q : quit the program\n"
	  "     h or ? : This help\n"
	  "     c : Copyrigth notice\n"
	  );
   line();
}

int main(int argc, char *argv[])
{
   int optct;
   copyright();
   assert(sizeof(int)==4);
   assert(sizeof(long)==4);
   assert(sizeof(long long)==8);
   // Parsing the arguments
   optct=optionProcess(&cbpmplayerOptions,argc,argv);
   if (argc-optct!=1) USAGE(EXIT_FAILURE);
   if (!HAVE_OPT(NORMAL)) USAGE(EXIT_FAILURE);
   dsp=OPT_ARG(DEVICE);
   currentperiod=targetperiod=normalperiod=atoi(OPT_ARG(NORMAL))*4;
   if (HAVE_OPT(TARGET))
     currentperiod=targetperiod=atoi(OPT_ARG(TARGET))*4;
   latency=atoi(OPT_ARG(LATENCY));
   latency*=WAVRATE;
   latency/=1000;
   rl_ttyset(0);
   if (WAVRATE==22050)
     {
	currentperiod/=2;
	normalperiod/=2;
	targetperiod/=2;
     }
   if (HAVE_OPT(VERBOSE))
     {
	double normaltempo;
	double targettempo;
	normaltempo=4.0*(double)WAVRATE*60.0/(double)normalperiod;
	targettempo=4.0*(double)WAVRATE*60.0/(double)targetperiod;
	printf("Normal tempo = %g BPM\n",normaltempo);
	printf("Target tempo = %g BPM",targettempo);
	printf(" speed(%g)\n",(double)normalperiod/(double)currentperiod);
     }
   // open dsp device
   open_outfile(dsp);
   // open input file
   open_infile(songname=argv[optct]);
   // read file and copy to dsp through a DDA
   fillbuffer();
   // close everything
   close(outfile);
   fclose(infile);
   rl_ttyset(1);
}
