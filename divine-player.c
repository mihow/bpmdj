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

/*-------------------------------------------
 *         Todo
 *-------------------------------------------
 * - playing < zero check when going forward/backward 
 * - % indicator
 * - jumping back to position
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

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define            inbufsiz (64*1024)
              long dmabufsiz=0;
  signed long long targetperiod;
  signed long long currentperiod;
  signed long long normalperiod;
  signed long long y,x=0;
               int start=1;
               int stop=0;

/*-------------------------------------------
 *         File output oprations
 *-------------------------------------------*/
int outfile;
void open_outfile()
{
   int p;
   audio_buf_info ab;
   outfile=open("/dev/dsp",O_WRONLY,0);
   if (!outfile)
     {
	printf("unable to open /dev/dsp\n");
	exit(2);
     }
   p=16;
   ioctl(outfile,SOUND_PCM_WRITE_BITS,&p);
   p=2;
   ioctl(outfile,SOUND_PCM_WRITE_CHANNELS,&p);
   p=44100;
   /* dit is echt wel stom omdat de geluidskaart dat toch niet aankan, 
    * die gaat gewoon wat brol skippen. 
    * We gaan dus m.a.w zaken genereren die genegeert gaan worden.
    * (yippie). Het is wel een goede test om te zien wat er gebeurt
    * in dergelijke situatie. De input van de gebruiker blijft een 
    * pak langer uitgestelt.
    */
   ioctl(outfile,SOUND_PCM_WRITE_RATE,&p);
   ioctl(outfile,SNDCTL_DSP_GETOSPACE,&ab);
   printf("Dsp device has %d bytes frees\n",ab.bytes);
   dmabufsiz=ab.bytes/4;
}

unsigned long long dmaplaycount()
{
   count_info cnt;
   ioctl(outfile,SNDCTL_DSP_GETOPTR,&cnt);
   printf("playcount = %d\n",cnt.bytes/4);
   return cnt.bytes/2;
}

int dmabufferpos()
{
   count_info cnt;
   ioctl(outfile,SNDCTL_DSP_GETOPTR,&cnt);
   //   printf("bufferpos = %d\n",cnt.ptr/4);
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
   else if (c=='1')
     {
	y-=currentperiod;
	printf("-1 Measure\n");
     }
   else if (c=='2')
     {
	y+=currentperiod;
	printf("+1 Measure\n");
     }
   else if (c=='4')
     {
	y-=currentperiod*4;
	printf("-4 Measure\n");
     }
   else if (c=='5')
     {
	y+=currentperiod*4;
	printf("+4 Measure\n");
     }
   else if (c=='7')
     {
	y-=currentperiod*8;
	printf("-8 Measure\n");
     }
   else if (c=='8')
     {
	y+=currentperiod*8;
	printf("+8 Measure\n");
     }
   else if (c=='3')
     {
	markat=dmaplaycount();
	printf("Marking master beat\n");
     }
   else if (c=='6')
     {
	unsigned long long fixit;
	unsigned long long measures;
	markat=dmaplaycount()-markat;
	printf("difference = %d\n",markat);
	markat%=currentperiod*8;
	printf("measure normalised = %d; %g\n",(long)markat,((double)markat/(double)currentperiod));
	fixit=markat; //+currentperiod/(long long)8;
	printf("adding 1/2 beat = %d\n",(long)fixit);
	fixit/=currentperiod/4;
	printf("results in %d beats wrong (%d)\n",(long)fixit,(long)currentperiod/4);
	fixit*=currentperiod/4;
	printf("Fixing slave beat: %d miss\n",(long)4*fixit/currentperiod,(long)(fixit-markat));
	y+=fixit;
     }
   else if (c=='/')
     {
	printf("Marking target beat; start 8 measures earlier\n");
	markat=y-currentperiod*8;
     }
   else if (c=='9')
     {
	signed long long fixit;
	printf("Started at target beat\n");
	fixit=(y%currentperiod)-(markat%currentperiod);
	y=markat+fixit;
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
   else if (c==';')
     {
	changetempo(currentperiod*1.05946*1.05946*1.05946);
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
   else if (c=='[')
     {
	printf("Back to the beginning\n");
	y=0;
     }
   else if (c=='{')
     {
	printf("Back to the beginning, normal tempo\n");
	y=0;
	x=0;
	changetempo(normalperiod);
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
   int result;
   unsigned long value[1];
   if (start) start=0;
   while(!stop)
     {
	x=y*normalperiod/currentperiod;
	value[0]=readat(x);
	y++;
	write(outfile,value,4);
     }
}

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
void copyright()
{
   printf("Divine Intervention Player v0.5, Copyright (c) 2001 Werner Van Belle\n");
   printf("This software is distributed under the GPL2 license. See Gpl2.txt for details\n");
   printf("See BeatMixing.ps for details how to use the program.\n");
   printf("--------------------------------------------------------------------\n");
}

int main(int argc, char *argv[])
{
   unsigned long long targetsize;
   unsigned long long realsize;
   rl_ttyset(0);
   copyright();
   printf("size of int(%d), long(%d), long long(%d)\n",sizeof(int),sizeof(long),sizeof(long long));
   // Parsing the arguments
   if (argc!=4)
     {
	printf("Expecting 3 argument(s)\n");
	exit(1);
     }
   currentperiod=targetperiod=atoi(argv[2])*4;
   normalperiod=atoi(argv[3])*4;
   printf("Playing at %g speed\n",(double)normalperiod/(double)currentperiod);
   // open dsp device
   open_outfile();
   // open input file
   open_infile(argv[1]);
   // read file and copy to dsp through a DDA
   realsize=fsize(infile)/4;
   targetsize=(realsize*targetperiod)/normalperiod;
   printf("File length (in samples) from %dk to %dk\n",(long)realsize/(long)(1024),(long)targetsize/(long)(1024));
   fillbuffer();
   // close everything
   close(outfile);
   fclose(infile);
   rl_ttyset(1);
}
	
