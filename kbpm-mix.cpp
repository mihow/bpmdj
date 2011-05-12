/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "mixerdialog.logic.h"
#include "dsp-drivers.h"
#include "dsp-oss.h"
#include "dsp-alsa.h"
#include "dsp-none.h"
#include "memory.h"
#include "scripts.h"
#include "dsp-mixed.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int    opt_quiet;
char*   arg_latency = "300";
static int    arg_posx = -1;
static int    arg_posy = -1;
static dsp_driver *dsp1;
static dsp_driver *dsp2;


void wait_for_unpause()
{
  return;
}

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
MixerDialogLogic *mixer_window;

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
  mixer_window = new MixerDialogLogic();
  if (arg_posx >=0 && arg_posy >=0)
    mixer_window->move(arg_posx,arg_posy);
  app->setMainWidget(mixer_window);
  mixer_window->show();
  pthread_create(y,NULL,go,NULL);
}

void options_failure(char* err)
{
  QMessageBox::critical(NULL,
			"Command line arguments",
			QString(err),
			QMessageBox::Ok,QMessageBox::NoButton);
  
  // print options
  printf("Usage:  kbpm-mix <options>\n\n"
	 "   -q          --quiet               be quiet\n"
	 "   -m arg      --match arg           song to match tempo with\n"
	 "   -p nbr nbr  --position nbr nbr    position to place the window\n"
	 OSS_OPTION_HELP
	 ALSA_OPTION_HELP
	 " if you specify more than one dsp device then the order of \n"
	 " parameters is important. First specify all the parameters \n"
	 " of one device, then all the arguments to the second. E.g: \n"
	 "  ./kbpm-mix --alsa --dev hw:0 --alsa --dev hw:1\n"
	 );
  exit(1);
}

#define toggle(name) name=1;
#define next_arg if (++i>=argc) options_failure("missing argument to last option");
#define arg_int(name) next_arg name=atoi(argv[i]);
#define arg_str(name) next_arg name=argv[i];
#define arg_float(name) next_arg name=atof(argv[i]);

static int dsps_chosen = 0;
void process_options(int argc, char* argv[])
{
  int i=1;
  int used = 0;
  dsp_driver * dsps[3];
  dsps[0]=dsps[1]=dsps[2]=0;
  dsp_driver *dsp = new dsp_none();
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
	  if (dsp && (used=dsp->parse_option(arg,argv[i+1])))
	    {
	      i+=used-1;
	    }
	  else if (option(arg,"quiet","q"))
	    opt_quiet=true;
#ifdef COMPILE_OSS
	  else if (option(arg,"oss")) 
	    {
	      dsp = new dsp_oss();
	      dsps[dsps_chosen++]=dsp;
	    }
#endif
#ifdef COMPILE_ALSA
	  else if (option(arg,"alsa"))
	    {
	      dsp = new dsp_alsa();
	      dsps[dsps_chosen++]=dsp;
	    }
#endif
	  else if (option(arg,"position","p"))
	    {
	      arg_int(arg_posx); 
	      arg_int(arg_posy); 
	    }
	}
    }
  if (dsps_chosen==0) 
    options_failure("You must supply a dsp device\n");
  dsp1 = dsps[0];
  dsp2 = dsps[1];
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

void set_color1(int r, int g, int b)
  {
    QColor c(r,g,b);
    mixer_window->Ch1MainText->setPaletteBackgroundColor(c);
    mixer_window->Ch1MonitorText->setPaletteBackgroundColor(c);
  }

void set_color2(int r, int g, int b)
  {
    QColor c(r,g,b);
    mixer_window->Ch2MainText->setPaletteBackgroundColor(c);
    mixer_window->Ch2MonitorText->setPaletteBackgroundColor(c);
  }

static mixarea * area1 = NULL;
static mixarea * area2 = NULL;
static synced_stereo_sample2* audio1;
static synced_stereo_sample2* audio2;
static int round_robin_size = 10000;

void one_card()
{
  // dsp start
  dsp1->open();
  dsp1->start();
  // measure the latency per device every second
  unsigned4 ticker = 0;
  unsigned8 latency = 0;
  unsigned4 latency_count = 0;
  signed4 delta1 = 1;
  signed4 delta2 = 1;

  while(!mixer_window->stop)
    {
      // we read the different values
      unsigned4 readat1 = area1->read;
      unsigned4 readat2 = area2->read;
      synced_stereo_sample2 value1 = audio1[readat1];
      synced_stereo_sample2 value2 = audio2[readat2];
      signed4 diff = value1.sync - value2.sync;
      stereo_sample2 value;
      
      // take care to route the signals
      switch (mixer_window->mode)
	{
	case 0:
	  value.left =
	    ((signed4)value1.value.left * mixer_window->ch1main 
	     + (signed4)value2.value.left * mixer_window->ch2main) / dspmixdoublemult;
	  value.right =
	    ((signed4)value1.value.right * mixer_window->ch1main 
	     + (signed4)value2.value.right * mixer_window->ch2main) / dspmixdoublemult;
	  break;
	case 1:
	  value1.value.left = 
	    ((signed4)value1.value.left +
	     (signed4)value1.value.right) / 2;
	  value2.value.left = 
	    ((signed4)value2.value.left +
	     (signed4)value2.value.right) / 2;
	  value.left =
	    (((signed4)value1.value.left) * mixer_window->ch1main +
	     ((signed4)value2.value.left) * mixer_window->ch2main) / dspmixdoublemult;
	  value.right =
	    (((signed4)value1.value.left) * mixer_window->ch1monitor +
	     ((signed4)value2.value.left) * mixer_window->ch2monitor) / dspmixdoublemult;
	  break;
	case 2:
	  value1.value.left = 
	    ((signed4)value1.value.left +
	     (signed4)value1.value.right) / 2;
	  value2.value.left = 
	    ((signed4)value2.value.left +
	     (signed4)value2.value.right) / 2;
	  value.right =
	    (((signed4)value1.value.left) * mixer_window->ch1main +
	     ((signed4)value2.value.left) * mixer_window->ch2main) / dspmixdoublemult;
	  value.left =
	    (((signed4)value1.value.left) * mixer_window->ch1monitor +
	     ((signed4)value2.value.left) * mixer_window->ch2monitor) / dspmixdoublemult;
	  break;
	default:
	  assert(0);
	}
      
      // write out this value & measure latency
      dsp1->write(value);
      if (++ticker == 44100)
	{
	  ticker=0;
	  latency += dsp1->latency();
	  latency_count ++;
	  signed4 meanlatency = latency / latency_count;
	  area1->latency = meanlatency;
	  area2->latency = meanlatency;
	  mixer_window->latency_audio->setText(QString::number((unsigned4)(latency/latency_count)*1000/44100));
	  mixer_window->latency_buffer->setText(QString::number(round_robin_size*1000/44100));
	  mixer_window->synchronisation->setText(QString::number((float)diff/(float)sync_max));
	  //printf("mean latency = %d; difference = %d\n",,diff);
	}

      // automatic synchronisation
#define status_check(nr) \
      mixstate status##nr = area##nr->status; \
      if (status##nr==mix_start)\
	{\
	  set_color##nr(0,255,0);\
	  area##nr -> status=status##nr=mix_playing;\
	}\
      if (status##nr==mix_stop)\
	{\
	  set_color##nr(255,0,0);\
	  area##nr -> status=status##nr=mix_stopped;\
	}\
      if (status##nr==mix_pause)\
	{\
	  set_color##nr(255,128,0);\
	  area##nr -> status=status##nr=mix_paused;\
	}\
      if (status##nr==mix_sync)\
	{\
	  set_color##nr(255,255,0);\
	  area##nr -> status=status##nr=mix_syncing;\
	}

      status_check(1);
      status_check(2);

#define sync_check(nr1,nr2,sign)\
      if (status##nr1==mix_syncing)\
	{ diff = sign diff;\
	  if ((diff < 2 && diff > - 2) || status##nr2!=mix_playing || !mixer_window->sync)\
	    {\
	      area##nr1->status = mix_playing;\
	      delta##nr1 = 1;\
	      set_color##nr1(0,255,0);\
	    }\
	  else \
	    {\
	      if (abs(diff) > sync_halve)\
		{\
		  if (diff < 0) diff += sync_max;\
		  else diff -= sync_max;\
		}\
	      if (diff < 0) delta##nr1 = 2;\
	      else delta##nr1 = 0;\
	    }\
	}

      sync_check(1,2,+) else sync_check(2,1,-);

      // move the read pointer, take care not to move the read pointer over the current write pointer
#define move_read_write(nr) \
      for(int i = 0 ; i < delta##nr ; i ++)\
	{\
	  readat##nr++;\
	  readat##nr%=round_robin_size;\
	  if ((unsigned4)readat##nr != (unsigned4)area##nr->write) \
	    area##nr->read = readat##nr;\
	  else \
	    break;\
	}

      move_read_write(1);
      move_read_write(2);
    };
  printf("Received stop event...\n");
  mixer_window->stopped=true;
}

void two_card()
{
  // start dsps
  printf("Opening dsp1\n");
  dsp1->open();
  printf("Opening dsp2\n");
  dsp2->open();
  printf("Starting dsp1\n");
  dsp1->start();
  printf("Starting dsp2\n");
  dsp2->start();
  // measure the latency per device every second
  unsigned4 ticker = 0;
  unsigned8 latency1 = 0;
  unsigned8 latency2 = 0;
  unsigned4 latency_count = 0;
  signed4 delta1 = 1;
  signed4 delta2 = 1;
  signed4 skipping = 0;
  signed4 skipevery = 0;

  while(!mixer_window->stop)
    {
      // we read the different values
      unsigned4 readat1 = area1->read;
      unsigned4 readat2 = area2->read;
      synced_stereo_sample2 value1 = audio1[readat1];
      synced_stereo_sample2 value2 = audio2[readat2];
      signed4 diff = value1.sync - value2.sync;
      stereo_sample2 out1;
      stereo_sample2 out2;

      // route the signals
      out1.left =
	((signed4)value1.value.left * mixer_window->ch1main 
	 + (signed4)value2.value.left * mixer_window->ch2main) / dspmixdoublemult;
      out1.right =
	((signed4)value1.value.right * mixer_window->ch1main 
	 + (signed4)value2.value.right * mixer_window->ch2main) / dspmixdoublemult;
      out2.left =
	((signed4)value1.value.left * mixer_window->ch1monitor 
	 + (signed4)value2.value.left * mixer_window->ch2monitor) / dspmixdoublemult;
      out2.right =
	((signed4)value1.value.right * mixer_window->ch1monitor
	 + (signed4)value2.value.right * mixer_window->ch2monitor) / dspmixdoublemult;
      
      if (mixer_window->mode == 1)
	{
	  stereo_sample2 o = out1;
	  out1 = out2;
	  out2 = o;
      	}
      
      // write out values & measure latency
      // the problem with simple writing
      //if (latency2 > latency1)
      // out2.value=0;
      
      // if the latency1 of the first player is greater than the second then dsp1 forms 
      // the bottleneck. To avoid this it simply skips an output.
      if (skipping && ticker%skipevery==0 )
	{
	  if (latency1 < latency2)
	    dsp1->write(out1);
	  else 
	    dsp2->write(out2);
	  skipping--;
	}
      else
	{
	  dsp1->write(out1);
	  dsp2->write(out2);
	}
      
      // can we measure the latency difference between the two cards ?
      // is that the slowest dsp device will make sure that the second one its buffer slowly gets 
      // entirily empty. 
      // to solve this problem, we will put the data into another round robin buffer.
      // it will contain two pointers. One containing the current readpositions of the two devices
      // the one that lags behind is the slowest player. So, the thread that would pass the write 
      // pointer can simply fill up zeros are // the s
      //This can be remedied by skipping bytes at the overflowing dsp
      // however, to do that we need the ability to measure it. This can be done by measuring the
      // latency and dropping bytes when one of the two is lower than the other..
      // 
      // to solve this we will create two threads. Each thread will take input from a shared memory
      // and dump the output as fast as possible to the sound card. When a buffer is empty it is flagged
      // as underrunned, as a result, the remainder 
      if (++ticker == 44100)
	{
	  ticker=0;
	  latency1 += dsp1->latency();
	  latency2 += dsp2->latency();
	  latency_count ++;
	  unsigned8 latency = (latency1 + latency2) / 2;
	  signed4 meanlatency = latency / latency_count;
	  area1->latency = meanlatency;
	  area2->latency = meanlatency;
	  mixer_window->latency_audio->setText(QString::number((unsigned4)(meanlatency)*1000/44100));
	  mixer_window->latency_buffer->setText(QString::number(round_robin_size*1000/44100));
	  mixer_window->synchronisation->setText(QString::number((float)diff/(float)sync_max));
	  mixer_window->latency_difference->setText(QString::number(((float)latency1 - (float)latency2)/(float)latency_count));
	  skipping = labs(latency1-latency2)/2;
	  if (skipping > 0)
	    skipevery = 44100 / skipping;
	  else
	    skipevery += 88000;
	}
      
      // automatic synchronisation
#define status_check(nr) \
      mixstate status##nr = area##nr->status; \
      if (status##nr==mix_start)\
	{\
	  set_color##nr(0,255,0);\
	  area##nr -> status=status##nr=mix_playing;\
	}\
      if (status##nr==mix_stop)\
	{\
	  set_color##nr(255,0,0);\
	  area##nr -> status=status##nr=mix_stopped;\
	}\
      if (status##nr==mix_pause)\
	{\
	  set_color##nr(255,128,0);\
	  area##nr -> status=status##nr=mix_paused;\
	}\
      if (status##nr==mix_sync)\
	{\
	  set_color##nr(255,255,0);\
	  area##nr -> status=status##nr=mix_syncing;\
	}

      status_check(1);
      status_check(2);

#define sync_check(nr1,nr2,sign)\
      if (status##nr1==mix_syncing)\
	{ diff = sign diff;\
	  if ((diff < 2 && diff > - 2) || status##nr2!=mix_playing || !mixer_window->sync)\
	    {\
	      area##nr1->status = mix_playing;\
	      delta##nr1 = 1;\
	      set_color##nr1(0,255,0);\
	    }\
	  else \
	    {\
	      if (abs(diff) > sync_halve)\
		{\
		  if (diff < 0) diff += sync_max;\
		  else diff -= sync_max;\
		}\
	      if (diff < 0) delta##nr1 = 2;\
	      else delta##nr1 = 0;\
	    }\
	}

      sync_check(1,2,+) else sync_check(2,1,-);

      // move the read pointer, take care not to move the read pointer over the current write pointer
#define move_read_write(nr) \
      for(int i = 0 ; i < delta##nr ; i ++)\
	{\
	  readat##nr++;\
	  readat##nr%=round_robin_size;\
	  if ((unsigned4)readat##nr != (unsigned4)area##nr->write) \
	    area##nr->read = readat##nr;\
	  else \
	    break;\
	}

      move_read_write(1);
      move_read_write(2);
    };

  printf("Received stop event...\n");
  mixer_window->stopped=true;
}

int main(int argc, char *argv[])
{
  app = new QApplication(argc,argv);
  process_options(argc,argv);
  terminal_start();
  // create devices and open for readonly
  int devfd1 = shm_open("/kbpm-mix1",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR); 
  if (devfd1 == -1)
    printf("Could not open shared memory area for device 1\n");
  int devfd2 = shm_open("/kbpm-mix2",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
  if (devfd2 == -1)
    printf("Could not open shared memory area for device 2\n");
  int buffersize = round_robin_size * sizeof(struct synced_stereo_sample2) + sizeof(mixarea);
  int mapsize = buffersize / getpagesize();
  if (mapsize * getpagesize() != buffersize)
    mapsize++;
  mapsize*=getpagesize();
  area1 = (mixarea *) mmap(NULL,mapsize,PROT_READ|PROT_WRITE,MAP_SHARED,devfd1,0);
  if (!area1 || area1 == MAP_FAILED)
    {
      printf("Could not map shared area 1 into memory\n");
      printf("Error is %s\n",strerror(errno));
    }
  area2 = (mixarea *) mmap(NULL,mapsize,PROT_READ|PROT_WRITE,MAP_SHARED,devfd2,0);
  if (!area2 || area2 == MAP_FAILED)
    printf("Could not map shared area 2 into memory\n");
  ftruncate(devfd1,mapsize);
  ftruncate(devfd2,mapsize);
  mlock(area1,mapsize);
  mlock(area2,mapsize);
  audio1 = (synced_stereo_sample2*)(area1+1);
  audio2 = (synced_stereo_sample2*)(area2+1);
  area1->init(round_robin_size);
  area2->init(round_robin_size);

  // do the actual input output loop
  if (dsps_chosen == 1)
    one_card();
  else
    {
      mixer_window->modebox->clear();
      mixer_window->modebox->insertItem("Dsp1 = Main; Dsp2 = Monitor");
      mixer_window->modebox->insertItem("Dsp1 = Monitor; Dsp2 = Main");
      two_card();
    };

  // inform both players that a stop is necessary
  area1->status=mix_halt;
  area2->status=mix_halt;
}
