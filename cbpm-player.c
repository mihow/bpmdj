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
 *         Headers
 *-------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include "cbpm-playeropts.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define            WAVRATE  (22050)
#define            wave_bufsize (32*1024)
  signed long long targetperiod;
  signed long long currentperiod;
  signed long long normalperiod;
  signed long long latency;
  signed long long y,x=0;
               int stop=0;
               int paused=0;

/*-------------------------------------------
 *         Clock oprations
 *-------------------------------------------*/
static clock_t starttick;
void clock_start()
{
   starttick=times(NULL);
}

signed long long clock_ticks()
{
   long long ticks=(long long)times(NULL)-(long long)starttick;
   long long answer=ticks*(long long)WAVRATE/(long long)CLK_TCK;
   assert(answer>=0);
   return answer;
}

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
static              int dsp;
static signed long long dsp_writecount=0;

void dsp_start()
{
   dsp_writecount=0;
}

signed long long dsp_playcount()
{
   count_info cnt;
   ioctl(dsp,SNDCTL_DSP_GETOPTR,&cnt);
   return cnt.bytes/4;
}

void dsp_pause()
{
   ioctl(dsp,SNDCTL_DSP_RESET);
   while(paused);
   dsp_writecount=0;
}

void dsp_write(unsigned long *value)
{
   dsp_writecount++;
   write(dsp,value,4);
}

signed long long dsp_latency()
{ 
   return dsp_writecount-dsp_playcount();
}

void dsp_open()
{
   int p;
   double latency;
   audio_buf_info ab;
   dsp=open(OPT_ARG(DEVICE),O_WRONLY,0);
   if (!dsp)
     {
	printf("unable to open /dev/dsp\n");
	exit(2);
     }
   p=16;
   ioctl(dsp,SOUND_PCM_WRITE_BITS,&p);
   p=2;
   ioctl(dsp,SOUND_PCM_WRITE_CHANNELS,&p);
   p=WAVRATE;
   ioctl(dsp,SOUND_PCM_WRITE_RATE,&p);
   ioctl(dsp,SNDCTL_DSP_GETOSPACE,&ab);
   if (HAVE_OPT(DEBUGLATENCY))
     {
	latency=(double)ab.bytes;
	latency/=4.0;
	latency/=(double)WAVRATE;
	latency*=1000.0;
	printf("Dsp device has %d bytes: latency = %g ms\n",ab.bytes,latency);
	dsp_start();
	clock_start();
	sleep(1);
	printf("Clock gives %d ticks (should be around %d)\n",(long)clock_ticks(),WAVRATE);
     }
}

void dsp_flush()
{
   ioctl(dsp,SNDCTL_DSP_SYNC);
}

void dsp_close()
{
   close(dsp);
}


/*-------------------------------------------
 *         File input oprations
 *-------------------------------------------*/
         char * wave_name=NULL;
unsigned long   wave_buffer[wave_bufsize];
         FILE * wave_file;
unsigned long   wave_bufferpos=-wave_bufsize;

static unsigned long fsize(FILE* wtf)
{
   unsigned long res;
   fseek(wtf,0,SEEK_END);
   res=ftell(wtf);
   fseek(wtf,0,SEEK_SET);
   return res;
}

void wave_open(char* fname)
{
   wave_name=fname;
   wave_file=fopen(fname,"rb");
   if (!wave_file) 
     {
	printf("unable to open %s\n",fname);
	exit(3);
     }
}

void wave_close()
{
   fclose(wave_file);
}

unsigned long wave_max()
{
   return fsize(wave_file)/4;
}

int wave_read(unsigned long pos, unsigned long *val)
{
   if (pos-wave_bufferpos>=wave_bufsize)
     {
	if (pos>wave_max()) return -1;
	wave_bufferpos=pos;
	fseek(wave_file,pos*4,SEEK_SET);
	fread(wave_buffer,4,wave_bufsize,wave_file);
     }
   *val=wave_buffer[pos-wave_bufferpos];
   return 0;
}


/*-------------------------------------------
 *         Synth operations
 *-------------------------------------------*/
        unsigned long long  lfo_phase=0;
          signed long long  lfo_period=0;
typedef unsigned long       (*_lfo_)(unsigned long x);
                     _lfo_  lfo_do;

void lfo_set(char* name, _lfo_ l, unsigned long long freq, unsigned long long phase)
{
   lfo_phase=phase;
   lfo_period=4*currentperiod/freq;
   lfo_do=l;
   printf("Switched to %s lfo at 4M/%d\n",name,freq);
}

unsigned long lfo_no(unsigned long x)
{
   return x;
}

typedef union
{
   unsigned long int value;
   struct 
     {
	signed short int left;
	signed short int right;
     } leftright;
} longtrick;

unsigned long lfo_volume(unsigned long x, signed long multiplierleft, signed long divisorleft, signed long multiplierright,signed long divisorright)
{
   longtrick lt;
   signed long int left;
   signed long int right;
   lt.value=x;
   left=lt.leftright.left;
   right=lt.leftright.right;
   left*=multiplierleft;
   right*=multiplierright;
   left/=divisorleft;
   right/=divisorright;
   lt.leftright.left=left;
   lt.leftright.right=right;
   return lt.value;
}

unsigned long lfo_pan(unsigned long x)
{
   signed long long diff;
   signed long quart;
   diff=(signed long long)y-(signed long long)lfo_phase;
   diff=diff%lfo_period;
   quart=lfo_period/4;
   /* at position 0, centre */
   if (diff<quart)
     return lfo_volume(x,quart-diff,quart,1,1);
   /* at position 0.25, right */
   diff-=quart;
   if (diff<quart)
     return lfo_volume(x,diff,quart,1,1);
   /* at position 0.5 centre */
   diff-=quart;
   if (diff<quart)
     return lfo_volume(x,1,1,quart-diff,quart);
   /* at position 0.75 left */
   diff-=quart;
   return lfo_volume(x,1,1,diff,quart);
}


unsigned long lfo_saw(unsigned long x)
{
   signed long long diff;
   diff=(signed long long)y-(signed long long)lfo_phase;
   diff=diff%lfo_period;
   return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

unsigned long lfo_break(unsigned long x)
{
   signed long long diff;
   diff=(signed long long)y-(signed long long)lfo_phase;
   diff=diff%lfo_period;
   if (diff>lfo_period*95/100) 
     return 0;
   else 
     return x;
}

unsigned long lfo_revsaw(unsigned long x)
{
   signed long long diff;
   diff=(signed long long)y-(signed long long)lfo_phase;
   diff=diff%lfo_period;
   diff=lfo_period-diff;
   return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

void lfo_init()
{
   lfo_do=lfo_no;
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
   currentperiod = period;
   y = x * currentperiod / normalperiod;
}

void printpos(char* text)
{
   unsigned long m=wave_max();
   printf("%s: %d % (%d/%d/%d)\n",text,(long)(y*100/m),(long)dsp_playcount()/1024,(long)y/1024,(long)m/1024);
}

void rewindto(unsigned long long qm, signed long long mes)
{
   unsigned long long gotopos=qm-currentperiod*mes;
   signed long long fixit=(y%currentperiod)-(gotopos%currentperiod);
   printf("Started at marked beat -%d measures\n",mes);
   y=gotopos+fixit; 
}

void useraction(int neglect)
{
   static unsigned long long playing_mark=0;     // the mark at the beginning (playing end) of the Q
   static unsigned long long queue_mark=0;       // the mark at the end of the Q
   static unsigned long long dma_mark=0;         // the dma time mark
   static unsigned long      time_mark=0;        // the clock time mark
   static   signed long long tempo_fade=0;       // 0 = targetperiod; 10 = normalperiod
   int c=0,nr=read(0,&c,1);
   switch (c)
     {
      case  0  :  return;
	/* forward backward movement */
      case '*' :  y+=currentperiod/(8);  break;
      case '\n':  y+=currentperiod/(4);  break;
      case '+' :  y-=currentperiod/(4*32);  break;
      case '-' :  y+=currentperiod/(4*32);  break;
      case '1' :  y-=currentperiod;  printpos("-1 Measure");  break;
      case '2' :  y+=currentperiod;  printpos("+1 Measure");  break;
      case '4' :  y-=currentperiod*4; printpos("-4 Measure"); break;
      case '5' :  y+=currentperiod*4; printpos("+4 Measure"); break;
      case '7' :  y-=currentperiod*8; printpos("-8 Measure"); break;
      case '8' :  y+=currentperiod*8; printpos("+8 Measure"); break;
	/* lfo's */
      case 'n' :  lfo_set("No",lfo_no,4,y-dsp_latency());  break;
      case 's' :  lfo_set("Saw",lfo_saw,8,y-dsp_latency());  break;
      case 'p' :  lfo_set("Pan",lfo_pan,8,y-dsp_latency());  break;
      case 'S' :  lfo_set("Saw",lfo_saw,16,y-dsp_latency());  break;
      case 'P' :  lfo_set("Pan",lfo_pan,16,y-dsp_latency());  break;
      case 'b' :  lfo_set("Break",lfo_break,1,y-dsp_latency());  break;
      case 'r' :  lfo_set("Reverse saw",lfo_revsaw,8,y-dsp_latency());  break;
      case 'R' :  lfo_set("Reverse saw",lfo_revsaw,16,y-dsp_latency());  break;
        /* tempo changes */
      case '>' :
      case '0' :  if (tempo_fade<10) tempo_fade=tempo_fade+1;
	          printf("Tempo fade %d\n",tempo_fade);
  	          changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10); break;
      case '<' :  if (tempo_fade>0) tempo_fade=tempo_fade-1;
 	          printf("Tempo fade %d\n",tempo_fade);
	          changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10); break;
      case '.' :  changetempo(normalperiod);  printf("Tempo normal\n");  break;
      case ',' :  changetempo(targetperiod);  printf("Tempo target\n");  break;
      case 'l' :  changetempo(currentperiod/1.05946);  break;
      case 'k' :  changetempo(currentperiod*1.05946);  break;
	/* marking and jumping */
      case '[' :
      case '/' :  dma_mark=dsp_playcount();
	          queue_mark=y;
	          playing_mark=queue_mark-dsp_latency();
	          printf("Marking master position\n");
	          break;
      case ']' :  y-=dsp_playcount()-dma_mark;
	          printf("Fixing slave beat\n"); break;
      case ' ' :  if (!paused) y-=dsp_latency();
	          paused=~paused; break;
      case '9' :  if (paused)
	            {
		       paused=0;
		       y=playing_mark;
		    }
	          else rewindto(queue_mark,0);
	          break;
      case '6' :  rewindto(queue_mark,8); break;
      case '3' :  rewindto(queue_mark,16); break;
	/* misc */
      case 't' :  if (time_mark)
	            printf("Seconds elapsed sinds mark %d\n",time(NULL)-time_mark);
	          else
	            {
		       printf("Time marked\n");
		       time_mark=time(NULL);
		    }
	          break;
      case '{' :  printf("Back to the beginning, normal tempo\n");
	          x=y=0; changetempo(normalperiod); break;
      case '%' :  printf("Song: %s\n",wave_name);  printpos("Current position: "); break;
      case '?' :
      case 'h' :  help(); break;
      case 'q' :  stop=1;  break;
      case 'c' :  copyright();  break;
      default  :
     }
   
   if (y<0)
     {
	printf("y underflow, setting to zero\n");
	y=0;
     }
   useraction(neglect);
}

void read_write_loop()
{
   signed   long long selfplaycount=-latency;
   signed   long long latencycheck;
   unsigned long      value[1];
   lfo_init();
   dsp_start();
   clock_start();
   while(!stop)
     {
	// wait for pause
	if (paused) 
	  {
	     dsp_pause();
	     clock_start();
	     selfplaycount=-latency;
	  }
	// calculate value
	x=y*normalperiod/currentperiod;
	if (wave_read(x,value)<0)
	  {
	     printf("End of song, going back 4 measures\n");
	     y-=4*currentperiod;
	     wave_read(x,value);
	  };
	value[0]=lfo_do(value[0]);
	y=y+1;
	// wait before writing
	while(selfplaycount>clock_ticks()) ;
	selfplaycount++;
	// write value
	dsp_write(value);
     }
   if (HAVE_OPT(DEBUGLATENCY))
     {
	latencycheck=dsp_playcount();
	dsp_flush();
	printf("Actual playing latency = %d ms\n",(long)((clock_ticks()-latencycheck)*(long long)1000/(long long)WAVRATE));
     }
}

/*-------------------------------------------
 *         Terminal routines  
 *-------------------------------------------*/
void terminal_blurb(int Reset)
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

void terminal_start()
{
   terminal_blurb(0);
}

void terminal_stop()
{
   terminal_blurb(1);
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
	  "2) Positioning\n"
	  "     p or ' ' : pause\n"
	  "     [ or / : mark position in song\n"
	  "     ] : set monitorred song to _difference_ with mark\n"
	  "     9 : Go to position mark or unpause at mark\n"
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
	  "     t : marks/shows time\n"
	  "     % : current position and name\n"
	  "     q : quit the program\n"
	  "     h or ? : This help\n"
	  "     c : Copyrigth notice\n"
	  "5) Lfo\n"
	  "     s or S : slow or fast saw (///)\n"
	  "     r or R : slow or fast reversed saw (\\\\\\)\n"
	  "     p or P : slow or fast pan lfo\n"
	  "     b : break (----_)\n");
   line();
}

int main(int argc, char *argv[])
{
   int optct;
   copyright();
   assert(sizeof(signed short int)==2);
   assert(sizeof(int)==4);
   assert(sizeof(long)==4);
   assert(sizeof(long long)==8);
   // Parsing the arguments
   optct=optionProcess(&cbpmplayerOptions,argc,argv);
   if (argc-optct!=1) USAGE(EXIT_FAILURE);
   if (!HAVE_OPT(NORMAL)) USAGE(EXIT_FAILURE);
   currentperiod=targetperiod=normalperiod=atoi(OPT_ARG(NORMAL))*4;
   if (HAVE_OPT(TARGET))
     currentperiod=targetperiod=atoi(OPT_ARG(TARGET))*4;
   latency=atoi(OPT_ARG(LATENCY));
   latency*=WAVRATE;
   latency/=1000;
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
   terminal_start();
   dsp_open();
   wave_open(argv[optct]);
   read_write_loop();
   dsp_close();
   wave_close();
   terminal_stop();
}
