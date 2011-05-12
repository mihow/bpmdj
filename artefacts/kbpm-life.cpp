/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <qlcdnumber.h>
#include <qwidget.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <qcombobox.h>
#include <unistd.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <linux/soundcard.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "bpm-life.h"
#include "signals.h"
#include "fourier.h"
#include "stereo-sample2.h"
#include "memory.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int    opt_quiet;
static int    opt_latency;
char*   arg_latency = "300";
static int    opt_none = 0;
static int    opt_alsa = 0;
static int    opt_oss = 0;
static int    opt_batch = 0;
static int    arg_posx = -1;
static int    arg_posy = -1;
volatile int paused = 0;

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
static int            dsp_in = 1;

#define WAVRATE (11025)
/*FILE *dsp_in;*/
void dsp_open()
{
  dsp_in = 0;
}

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
BpmLife *mixer_window;

void * go(void* neglect)
{
  app->exec();
  delete(mixer_window);
  delete(app);
  return neglect;
}

void terminal_start()
{
  pthread_t *y = allocate(1,pthread_t);
  mixer_window = new BpmLife();
  if (arg_posx >=0 && arg_posy >=0)
    mixer_window->move(arg_posx,arg_posy);
  app->setMainWidget(mixer_window);
  mixer_window->show();
  pthread_create(y,NULL,go,NULL);
}

bool show_error(int err, int err2, const char*text)
{
  if (err==err2)
    {
      const QString a=QString("Error");
      const QString b=QString(text);
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      return true;
    }
  return false;
}


#define  mperiod2bpm(period) (4.0*(double)WAVRATE*60.0/(double)(period))

#define FACTOR 4

int main(int argc, char *argv[])
{
  app = new QApplication(argc,argv);
  terminal_start();
  dsp_open();
  //  int largest_delay = WAVRATE * 60 * FACTOR / 90;
  //  int smallest_delay = WAVRATE * 60 * FACTOR / 160;
  double wavrate = WAVRATE;
  int buffer_size = higher_power_of_two( 44100 * 1 );
  int smallest_freq =  buffer_size * 90 / (WAVRATE * 60);
  int highest_freq = buffer_size * 160 / (WAVRATE * 60);

  // f = b * wavrate / windowsize
  // b = f * windowsize / wavrate

  printf("buffer size = %d\nlargest = %d\nsmallest = %d\n",
	 buffer_size,highest_freq, smallest_freq);
  stereo_sample2 *buffer = allocate(buffer_size,stereo_sample2);
  double *doubles = allocate(buffer_size,double);
  double *doubles_out = allocate(buffer_size,double);
  double *doubles_o2 = allocate(buffer_size,double);
  while(true)
    {
      // 0. Read enough audio
      int toread = buffer_size*4;
      unsigned char * ptr = (unsigned char*)buffer;
      while(toread>0)
	{
	  int r = read(dsp_in,ptr,toread > 32768 ? 32768 : toread);
	  if (r>0)
	    {
	      ptr+=r;
	      toread-=r;
	    }
	  else
	    {
	      printf("error %s\n",strerror(errno));
	    }
	}
      // 1. Convert it to doubles
      for(int i = 0 ; i < buffer_size;i++)
	{
	  doubles[i]=fabs(buffer[i].left+buffer[i].right);
	}
      // 2. Analyze it
      fft_double(buffer_size,false,doubles,NULL,doubles_out,doubles_o2);
      //      biased_circular_autocorrelation(doubles,buffer_size*2);
      // 3. Find maximum
      double m = 0;
      int j;
      for(int i = smallest_freq ; i < highest_freq; i++)
	{
	  double d = (doubles_out[i]*doubles_out[i])+
	    (doubles_o2[i]*doubles_o2[i]);
	  if (d>m) 
	    {
	      m=d;
	      j=i;
	    }
	}
      //      return 0;
      // 4. Calculate back to the tempo
      double t = j * buffer_size * 60.0 / wavrate;
      //bin = wavrate * 160 / (buffer_size * 60)   
      //   double t = mperiod2bpm(j) * FACTOR / 4.0;
      //char s[200];
      //      sprintf(s,"%g",t);
      mixer_window->tempo->display(t);
      app->processEvents();
    }
}

/* Possible optimalisations:
 * - convert bytes while reading them in the buffer (combine step 1 into 0)
 * - analyze it using fftw
 * - analyze it using goertwel optimalisations in the algorithm
 * - search only within the necessary range
 * - keep a backlog of read data and increase the analysis buffer_size 
 *   depending on the possibilities offered by the processor
 * - take into account autocorrelations within the autocorrelation sequence
 */
