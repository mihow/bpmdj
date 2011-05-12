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

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif /* HAVE_TERMIOS_H */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif /* HAVE_LIBGEN_H */

#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#endif /* HAVE_LINUX_SOUNDCARD_H */

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif /* HAVE_SIGNAL_H */

#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif /* HAVE_ASSERT_H */

#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */

#include "cbpm-index.h"
#include "cbpm-playeropts.h"

#include "common.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define  WAVRATE  (22050)
#define  wave_bufsize (32*1024)
  signed8 targetperiod;
  signed8 currentperiod;
  signed8 normalperiod;
  signed8 latency;
  signed8 y,x=0;
     int stop=0;
     int paused=0;

unsigned8 x_normalise(unsigned8 y)
{
   return y*normalperiod/currentperiod;
}

unsigned8 y_normalise(unsigned8 x)
{
   return x*currentperiod/normalperiod;
}

/*-------------------------------------------
 *         Clock oprations
 *-------------------------------------------*/
static clock_t starttick;
void clock_start()
{
   starttick=times(NULL);
}

signed8 clock_ticks()
{
   signed8 ticks=(signed8)times(NULL)-(signed8)starttick;
   signed8 answer=ticks*(signed8)WAVRATE/(signed8)CLK_TCK;
   assert(answer>=0);
   return answer;
}

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
static    int dsp;
static signed8 dsp_writecount=0;

void dsp_start()
{
   dsp_writecount=0;
}

signed8 dsp_playcount()
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

void dsp_write(unsigned4 *value)
{
   dsp_writecount++;
   write(dsp,value,4);
}

signed8 dsp_latency()
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
	printf("Error: unable to open /dev/dsp\n");
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
unsigned4 wave_buffer[wave_bufsize];
FILE * wave_file;
unsigned4 wave_bufferpos=-wave_bufsize;

static unsigned4 fsize(FILE* wtf)
{
   unsigned4 res;
   fseek(wtf,0,SEEK_END);
   res=ftell(wtf);
   fseek(wtf,0,SEEK_SET);
   return res;
}

void wave_open(char* fname)
{
   char fn[500];
   // start decoding the file; this means: fork mp3 process
   // wait 1 second and start reading the file
   if (!fork())
     {
	char execute[500];
	sprintf(execute,"glue-mp3raw \"%s\"\n",fname);
	if (system(execute)<=256) exit(100);
	sprintf(execute,"./glue-mp3raw \"%s\"\n",fname);
	if (system(execute)<=256) exit(100);
	printf("Error: couldn't execute glue-mp3raw or ./glue-mp3raw\n");
	exit(100);
     }
   sleep(1);
   sprintf(fn,"%s.raw",fname);
   wave_name=strdup(basename(fn));
   wave_file=fopen(wave_name,"rb");
   if (!wave_file) 
     {
	printf("Error: unable to open %s\n",wave_name);
	exit(3);
     }
}

void wave_close()
{
   fclose(wave_file);
   remove(wave_name);
}

unsigned4 wave_max()
{
   return fsize(wave_file)/4;
}

int wave_read(unsigned4 pos, unsigned4 *val)
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
typedef unsigned4 (*_lfo_)(unsigned4 x);
        unsigned8   lfo_phase=0;
        signed8   lfo_period=0;
         _lfo_   lfo_do;

void jumpto(signed8, int);

void lfo_set(char* name, _lfo_ l, unsigned8 freq, unsigned8 phase)
{
   /* if paused, unpause and choose own phase */
   lfo_period=4*currentperiod/freq;
   lfo_do=l;
   if (paused)
     {
	jumpto(0,0);
	lfo_phase=y;
	paused=0;
	printf("Restarted with %s lfo at 4M/%d\n",name,freq);
     }
   else
     {
	lfo_phase=phase;
	printf("Switched to %s lfo at 4M/%d\n",name,freq);
     }
}

unsigned4 lfo_no(unsigned4 x)
{
   return x;
}

typedef union
{
   unsigned4 value;
   struct 
     {
	signed short int left;
	signed short int right;
     } leftright;
} longtrick;

unsigned4 lfo_volume(unsigned4 x, signed4 multiplierleft, signed4 divisorleft, signed4 multiplierright,signed4 divisorright)
{
   longtrick lt;
   signed4 left;
   signed4 right;
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

unsigned4 lfo_metronome(unsigned4 x)
{
   double val;
   signed8 diff;
   longtrick lt;
   signed4 left;
   signed4 right;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   /* say ping */
   val=sin(6.28*diff*(double)(WAVRATE)/440.0)*4096.0*(1.0-(double)diff/(double)lfo_period);
   /* mix them */
   lt.value=x;
   lt.leftright.left+=(signed short)val;
   lt.leftright.right+=(signed short)val;
   return lt.value;
}


unsigned4 lfo_difference(unsigned4 x)
{
   static signed short prevl;
   static signed short prevr;
   double val;
   signed8 diff;
   longtrick lt;
   signed4 left;
   signed4 right;
   lt.value=x;
   left=lt.leftright.left-prevl;
   right=lt.leftright.right-prevr;
   prevl=lt.leftright.left;
   prevr=lt.leftright.right;
   lt.leftright.left=left;
   lt.leftright.right=right;
   return lt.value;
}


unsigned4 lfo_pan(unsigned4 x)
{
   signed8 diff;
   signed4 quart;
   diff=(signed8)y-(signed8)lfo_phase;
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

unsigned4 lfo_saw(unsigned4 x)
{
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

unsigned4 lfo_break(unsigned4 x)
{
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   if (diff>lfo_period*95/100) 
     return 0;
   else 
     return x;
}

unsigned4 lfo_revsaw(unsigned4 x)
{
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   diff=lfo_period-diff;
   return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

void lfo_init()
{
   lfo_do=lfo_no;
}

/*-------------------------------------------
 *         Cue points
 *-------------------------------------------*/
/* these are expressed in sampleticks in the 
 * real (non-stretched) song
 */

typedef unsigned8 cue_info;
static cue_info cue_before = 0;
static cue_info cue = 0;
static cue_info cues[4] = {0,0,0,0};

void cue_set()
{
   cue=x_normalise(y-dsp_latency());
   printf("Setting cue\n");
}

void cue_shift(char* txt, signed8 whence)
{
   printf("%s\n",txt);
   cue+=whence;
   if (!paused) 
     paused=1;
}

void cue_store(char* text,int nr)
{
   printf("Storing %s cue\n",text);
   cues[nr]=cue;
}

void cue_retrieve(char* text,int nr)
{
   printf("Retrieving and jumping to %s cue\n",text);
   cue=cues[nr];
}

void cue_write()
{
   if (cue!=cue_before)
     {
	index_cue=cue;
	index_changed=1;
	printf("Index has been changed\n");
     }
   if (index_cue_z!=(unsigned4)cues[0])
     {
	printf("Z cue has been changed\n");
	index_changed=1;
	index_cue_z=cues[0];
     }
   if (index_cue_x!=(unsigned4)cues[1])
     {
	printf("X cue has been changed\n");
	index_changed=1;
	index_cue_x=cues[1];
     }
   if (index_cue_c!=(unsigned4)cues[2])
     {
	printf("C cue has been changed\n");
	index_changed=1;
	index_cue_c=cues[2];
     }
   if (index_cue_v!=(unsigned4)cues[3])
     {
	printf("V cue has been changed\n");
	index_changed=1;
	index_cue_v=cues[3];
     }
}

void cue_read()
{
   cue_before=cue=index_cue;
   cues[0]=index_cue_z;
   cues[1]=index_cue_x;
   cues[2]=index_cue_c;
   cues[3]=index_cue_v;
   if (!HAVE_OPT(QUIET))
     {
	printf("Available cue's: ");
	if (cues[0]) printf("Z ");
	if (cues[1]) printf("X ");
	if (cues[2]) printf("C ");
	if (cues[3]) printf("V ");
	printf("\n");
     }
   
}

/*-------------------------------------------
 *         Program logic
 *-------------------------------------------*/
void help();

void changetempo(signed8 period)
{
   /* change tempo to period
    * - the current x should remain the same
    * x = y * normalperiod / currentperiod
    * y = x * currentperiod / normalperiod
    */
   int change=(long)(currentperiod*1000/period)-(long)1000;
   if (period>currentperiod)
     printf("slow down (%d)\n",change);
   else 
     printf("speed up (%d)\n",change);
   
   
   currentperiod = period;
   y = x * currentperiod / normalperiod;
   
}

void printpos(char* text)
{
   unsigned4 m=wave_max();
   printf("%s: %d % (%d/%d)\n",text,
	  (long)(x_normalise(y)*100/m),
	  (long)x_normalise(y)/1024,
	  (long)m/1024);
}

void jumpto(signed8 mes, int txt)
{
   if (paused)
     {
	paused=0;
	y=y_normalise(cue);
	if (txt) printf("Res");
     }
   else
     {
	// - first we have the position which _should_ be playing 'NOW'
	// - second, we have the data which is now queued
	// - third we have the latency
	// - if we subtract the latency from the data being qued, we have the real playing position
	// - if we subtract what should be playing with what is playing, we have a difference
	// - we can fixed this difference to fit a measure
	// - this fixed difference is added to gotopos.
	signed8 gotopos=y_normalise(cue)-currentperiod*mes;
	signed8 difference=gotopos-y+dsp_latency();
	signed8 fixeddiff=(difference/currentperiod)*currentperiod;
	y+=fixeddiff;
	if (txt) printf("S");
     }
   if (txt)
     {
	if (mes)
	  printf("tarted at cue -%d measures\n",mes);
	else
	  printf("tarted at cue\n",mes);
     }
}

void useraction(int neglect)
{
   static unsigned4      time_mark=0;        // the clock time mark
   static   signed8 tempo_fade=0;       // 0 = targetperiod; 10 = normalperiod
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
      case 'D' :  lfo_set("Differentiator",lfo_difference,16,0); break;
	/* placing cues */
      case 'm' :  lfo_set("Metronome",lfo_metronome,16,y-dsp_latency()); break;
      case 'd' :  index_period*=2; index_changed=1; 
	          printf("Doubled the current period\n"); break;
      case 'u' :  index_period/=2; index_changed=1;
	          printf("Halved the current period\n"); break;
        /* tempo changes */
      case '>' :
      case '0' :  if (tempo_fade<10) tempo_fade=tempo_fade+1;
	          printf("Tempo fade %d ",tempo_fade);
  	          changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10); break;
      case '<' :  if (tempo_fade>0) tempo_fade=tempo_fade-1;
 	          printf("Tempo fade %d ",tempo_fade);
	          changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10); break;
      case '.' :  changetempo(normalperiod);  printf("Tempo normal\n");  break;
      case ',' :  changetempo(targetperiod);  printf("Tempo target\n");  break;
      case 'l' :  changetempo(currentperiod/1.05946);  break;
      case 'k' :  changetempo(currentperiod*1.05946);  break;
	/* marking and jumping */
      case '[' :
      case '/' :  cue_set(); break;
      case ']' :  y=y_normalise(cue)+dsp_latency();
	          printf("Bringing cue at hit\n"); break;
      case ' ' :  if (!paused)
	             {
			if (!cue) cue_set();
			paused=1;
		     }
	          else jumpto(0,0);
	          break;
      case '9' :  jumpto(0,1); break;
      case '6' :  jumpto(8,1); break;
      case '3' :  jumpto(16,1); break;
	/* cue's */
      case 'Z' :  cue_store("Z-",0); break;
      case 'X' :  cue_store("X-",1); break;
      case 'C' :  cue_store("C-",2); break;
      case 'V' :  cue_store("V-",3); break;
      case 'z' :  cue_retrieve("Z-",0); jumpto(0,0); break;
      case 'x' :  cue_retrieve("X-",1); jumpto(0,0); break;
      case 'c' :  cue_retrieve("C-",2); jumpto(0,0); break;
      case 'v' :  cue_retrieve("V-",3); jumpto(0,0); break;
	/* misc */
      case 't' :  if (time_mark)
	            printf("Seconds elapsed sinds mark %d\n",time(NULL)-time_mark);
	          else
	            {
		       printf("Time marked\n");
		       time_mark=time(NULL);
		    }
       	          break;
      case '{' :  cue_shift("Shifting cue 8 measure backward",-8*normalperiod); break;
      case '}' :  cue_shift("Shifting cue 8 measure forward",8*normalperiod); break;
      case '(' :  cue_shift("Shifting cue 1/32 note backward",-normalperiod/128); break;
      case ')' :  cue_shift("Shifting cue 1/32 note forward",+normalperiod/128); break;
      case '%' :  printf("Song: %s\n",wave_name);  printpos("Current position: "); break;
      case '?' :
      case 'h' :  help(); break;
      case 'q' :  stop=1; paused=0; break;
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
   signed8   selfplaycount=-latency;
   signed8   latencycheck;
   unsigned4 value[1];
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
	printf("Actual playing latency = %d ms\n",(long)((clock_ticks()-latencycheck)*(signed8)1000/(signed8)WAVRATE));
     }
}

/*-------------------------------------------
 *         Terminal routines  
 *-------------------------------------------*/
void (*oldio)(int);
void (*oldhup)(int);
void (*oldurg)(int);

void terminal_blurb(int Reset)
{
   static struct termios old;
   struct termios new;
   if (Reset == 0)
     {(void) tcgetattr (0, &old);
	new = old;
	new.c_lflag &= ~(ECHO | ICANON);
	new.c_iflag &= ~(ISTRIP | INPCK);
	(void) tcsetattr (0, TCSANOW, &new);}
   else (void) tcsetattr (0, TCSANOW, &old);
}

void stopsignal(int n)
{
   stop=1;
}

void terminal_start()
{
   long i;
   terminal_blurb(0);
   fcntl(0,F_SETFL,O_ASYNC | O_NONBLOCK);
   i=getpid();
   fcntl(0,F_SETOWN,i);
   oldio=signal(SIGIO, useraction);
   oldurg=signal(SIGURG, useraction);
   oldhup=signal(SIGHUP, stopsignal);
}

void terminal_stop()
{
   terminal_blurb(1);
   signal(SIGIO, oldio);
   signal(SIGURG, oldurg);
   signal(SIGHUP, oldhup);
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
   printf("This software is distributed under the GPL2 license. See copyright.txt for\n");
   printf("details. Press 'h' for Help. See beatmixing.ps for details\n");
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
	  "     Z, X, C, V: Store current cue\n"
	  "     z, x, c, v: Retrieve cue and jump to it\n"
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
   assert(sizeof(signed2)==2);
   assert(sizeof(int)==4);
   assert(sizeof(signed4)==4);
   assert(sizeof(signed8)==8);
   // Parsing the arguments
   optct=optionProcess(&cbpmplayerOptions,argc,argv);
   if (argc-optct!=1) USAGE(EXIT_FAILURE);
   if (HAVE_OPT(MATCH))
     {
	index_read(OPT_ARG(MATCH));
	targetperiod=index_period*4;
	index_free();
     }
   if (strstr(argv[optct],".mp3"))
     if (strcmp(strstr(argv[optct],".mp3"),".mp3")==0)
       {
	  printf("Error: please enter the index file, not the mp3 file\n");
	  exit(30);
       }
   index_read(argv[optct]);
   normalperiod=index_period*4;
   if (!HAVE_OPT(MATCH)) targetperiod=normalperiod;
   currentperiod=targetperiod;
   latency=atoi(OPT_ARG(LATENCY));
   latency*=WAVRATE;
   latency/=1000;
   if (WAVRATE==22050)
     {
	currentperiod/=2;
	normalperiod/=2;
	targetperiod/=2;
     }
   if (!HAVE_OPT(QUIET))
     {
	double normaltempo;
	double targettempo;
	normaltempo=4.0*(double)WAVRATE*60.0/(double)normalperiod;
	targettempo=4.0*(double)WAVRATE*60.0/(double)targetperiod;
	printf("Normal tempo = %g BPM\n",normaltempo);
	printf("Target tempo = %g BPM",targettempo);
	printf(" speed(%g)\n",(double)normalperiod/(double)currentperiod);
     }
   terminal_start();
   cue_read();
   dsp_open();
   wave_open(index_file);
   read_write_loop();
   dsp_close();
   wave_close();
   cue_write();
   terminal_stop();
   if (index_changed)
     {
	if (!HAVE_OPT(QUIET))
	  printf("Updating index file\n");
	index_write();
     }
   index_free();
}
