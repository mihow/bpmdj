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
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "player-core.h"
#include "dsp-oss.h"
#include "scripts.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
void help();

void printpos(char* text)
{
   unsigned4 m=wave_max();
   printf("%s: %d %% (%d/%d)\n",text,
	  (int)(x_normalise(y)*100/m),
	  (int)x_normalise(y)/1024,
	  (int)m/1024);
}

void useraction(int neglect)
{
   static unsigned4      time_mark=0;        // the clock time mark
   static   signed8 tempo_fade=0;            // 0 = targetperiod; 10 = normalperiod
   int c=0;
   read(0,&c,1);
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
      case 'd' :  doubleperiod();
	          printf("Doubled the current period\n"); break;
      case 'u' :  halveperiod();
	          printf("Halved the current period\n"); break;
        /* tempo changes */
      case '>' :
      case '0' :  if (tempo_fade<10) tempo_fade=tempo_fade+1;
	          printf("Tempo fade %d ",(int)tempo_fade);
  	          changetempo(targetperiod+(normalperiod - targetperiod)*tempo_fade/10); break;
      case '<' :  if (tempo_fade>0) tempo_fade=tempo_fade-1;
 	          printf("Tempo fade %d ",(int)tempo_fade);
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
	            printf("Seconds elapsed sinds mark %d\n",(int)(time(NULL)-time_mark));
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
     }
   
   if (y<0)
     {
	printf("y underflow, setting to zero\n");
	y=0;
     }
   useraction(neglect);
}

void show_error(char * msg)
  {
     printf("Error: %s\n",msg);
  }

/*-------------------------------------------
 *         Terminal routines  
 *-------------------------------------------*/
void (*oldio)(int);
void (*oldhup)(int);
void (*oldurg)(int);

void msg_slowdown(int change)
{
   printf("slow down (%d)\n",change);
}

void msg_speedup(int change)
{
   printf("speed up (%d)\n",change);
}

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
	  "     %% : current position and name\n"
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

void options_failure(char* err)
{
   // print options
   printf("Usage:  cbpm-play <options> argument\n\n"
	  "   -w       --debuglatency   debugs latency of the player\n"
	  "   -q       --quiet          be quiet\n"
	  "   -d arg   --dsp arg        dsp device to use (default = /dev/dsp)\n"
	  "   -m arg   --match arg      song to match tempo with\n"
	  "   -r arg   --rawpath arg    patrh that .raw files are stored in (default = ./)\n"
	  "   -l nbr   --latency nbr    required latency in ms (default = 744)\n"
	  "   argument                  the index file of the song to play\n\n%s\n\n",err);
   exit(1);
}

void process_options(int argc, char* argv[])
{
   // run trough all the arguments
   int i;
   for(i = 1 ; i < argc ; i ++)
     {
	if (argv[i][0]=='-')
	  {
	     char* arg;
	     // check long opt ?
	     if (argv[i][1]=='-')
	       arg=argv[i]+2;
	     else if (argv[i][1]==0 || argv[i][2]!=0)
	       options_failure("option neither short nor long");
	     else arg=argv[i]+1;
	     // check value
	     if (strcmp(arg,"quiet")==0 ||
		      strcmp(arg,"q")==0)
	       opt_quiet=1;
#ifdef COMPILE_OSS
	     else if (strcmp(arg,"dsp")==0 ||
		      strcmp(arg,"d")==0)
	       {
		  if (++i>=argc) 
		    options_failure("dsp argument scanning error");
		  arg_dsp=argv[i];
	       }
#endif
	     else if (strcmp(arg,"match")==0 ||
		      strcmp(arg,"m")==0)
	       {
		  opt_match=1;
		  if (++i>=argc) 
		    options_failure("match argument scanning error");
		  arg_match=argv[i];
	       }
	     else if (strcmp(arg,"latency")==0 ||
		      strcmp(arg,"l")==0)
	       {
		  opt_latency=1;
		  if (++i>=argc)
		    options_failure("latency argument scanning error");
		  arg_latency=argv[i];
	       }
	     else if (strcmp(arg,"rawpath")==0 ||
		      strcmp(arg,"r")==0)
	       {
		 if (++i>=argc)
		   options_failure("raw path argument scanning error");
		 arg_rawpath = argv[i];
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
   int err;
   process_options(argc,argv);

   // initialize core
   err = core_init(0);
   if (err == err_needidx)
     {
       printf("Error: please enter the index file, not the "SONG_EXT" file\n");
       printf("       an index file can be made with 'kbpm-play -c'\n");
       exit(err);
     }
   else if (err == err_noraw)
     {
       printf("Error: No raw file found\n");
       exit(err);
     }
   else if (err == err_nospawn)
     {
       printf("Error: unable to start decoding process. Check your PATH\n");
       exit(err);
     }
   // open core 
   err = core_open();
   //   if (err == err_mixer)
   //     {
   //       printf("Unable to open mixer device %s\n",arg_mixer);
   //       exit(err);
   //     }
   //else
   if (err == err_dsp)
     {
       printf("Unable to open dsp device\n");
       exit(err);
     }
   terminal_start();
   core_play();
   terminal_stop();
   core_close();
   core_done();
   return 0;
}
