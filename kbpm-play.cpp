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
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>
#include <linux/soundcard.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "songplayer.logic.h"
#include "kbpm-counter.h"
#include "spectrumanalyzer.logic.h"

extern "C"
{
#include "cbpm-index.h"
#include "common.h"
#include "player-core.h"
}

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int   opt_batch = 0;
static int   opt_create = 0;
static int   opt_color = 0;
static char* arg_posx = NULL;
static char* arg_posy = NULL;
static int   arg_low=120;
static int   arg_high=160;

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
void * go(void* neglect)
  {
     app->exec();
  }

void msg_slowdown(int change)
{
}

void msg_speedup(int change)
{
}

void terminal_start()
{
   pthread_t y;
   SongPlayerLogic *test=new SongPlayerLogic();
   if (arg_posx && arg_posy)
     test->move(atoi(arg_posx),atoi(arg_posy));
   app->setMainWidget(test);
   test->show();
   pthread_create(&y,NULL,go,NULL);
}


void terminal_stop()
{
   delete(app);
}

void options_failure(char* err)
{
   // print options
   printf("Usage:  kbpm-play <options> argument\n\n"
//	  "   -w          --debuglatency      debugs latency of the player\n"
          "   -c          --create            create an index file if none exists\n"
	  "   -q          --quiet             be quiet\n"
	  "   -d arg      --dsp arg           dsp device to use (default = /dev/dsp)\n" 
	  "   -x arg      --mixer arg         mixer device to use (default = /dev/mixer)\n"
	  "   -m arg      --match arg         song to match tempo with\n"
	  "   -L nbr      --latency nbr       required latency in ms (default = 744)\n"
	  "   -p nbr nbr  --position nbr nbr  position to place the window\n"
	  "   -b          --batch             immediatelly obtain bpmcount, no sound, quit immidiatelly\n"
	  "   -l nbr      --low nbr           lowest bpm to look for (default = 120)\n"
	  "   -h nbr      --high nbr          highest bpm to look for (default = 160)\n"
	  "   -s          --spectrum          immediatelly obtain color at cue-point, no sound, quit imm\n"
	  "   argument                        the index file of the song to handle\n\n%s\n\n",err);
   exit(1);
}

void process_options(int argc, char* argv[])
{
  int i=1;
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
	  if (strcmp(arg,"quiet")==0 ||
	      strcmp(arg,"q")==0)
	       opt_quiet=1;
	  else if (strcmp(arg,"batch")==0 ||
		   strcmp(arg,"b")==0)
	    opt_batch=1;
	  else if (strcmp(arg,"create")==0 ||
		   strcmp(arg,"c")==0)
	    opt_create=1;
	  else if (strcmp(arg,"spectrum")==0 ||
		   strcmp(arg,"s")==0)
	    opt_color=1;
	  else if (strcmp(arg,"dsp")==0 ||
		   strcmp(arg,"d")==0)
	    {
	      if (++i>=argc) 
		options_failure("dsp argument scanning error");
	      arg_dsp=argv[i];
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
	  else if (strcmp(arg,"match")==0 ||
		   strcmp(arg,"m")==0)
	    {
	      opt_match=1;
	      if (++i>=argc) 
		options_failure("match argument scanning error");
	      arg_match=argv[i];
	    }
	  else if (strcmp(arg,"latency")==0 ||
		   strcmp(arg,"L")==0)
	    {
	      opt_latency=1;
	      if (++i>=argc)
		options_failure("latency argument scanning error");
	      arg_latency=argv[i];
	    }
	  else if (strcmp(arg,"mixer")==0 ||
		   strcmp(arg,"x")==0)
	    {
	      if (++i>=argc)
		options_failure("mixer argument scanning error");
	      arg_mixer=argv[i];
	    }
	  else if (strcmp(arg,"position")==0 ||
		   strcmp(arg,"p")==0)
	    {
	      if (++i>=argc)
		options_failure("position x argument scanning error");
	      arg_posx=argv[i];
	      if (++i>=argc)
		options_failure("position y argument scanning error");
	      arg_posy=argv[i];
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

void show_error(char*text)
{
   const QString a=QString("Error");
   const QString b=QString(text);
   QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
}

bool checkRaw()
  {
    FILE * raw;
    char d[500];
    // the filename of the file to read is the basename 
    // suffixed with .raw
    sprintf(d,"%s.raw",basename(index_file));
  }

int main(int argc, char *argv[])
{
  process_options(argc,argv);
  // create an application
  app = new QApplication(argc,argv);
  // if we need to create an index file we'll make it.
  if (opt_create)
    {
      // argument aindigt normaal op .mp3 of iets in dien trend...
      char newname[500];
      strcpy(newname,argument);
      int len = strlen(newname);
      if (len>=4 && newname[len-4]=='.')
	strcpy(newname+len-3,"idx");
      else
	options_failure("Sorry, mp3 must end on .mp3");
      // create index and write it..
      index_init();
      index_setversion();
      index_readfrom = strdup(newname);
      index_file = strdup(argument);
      index_changed = 1;
      index_tags=strdup("New");
      index_period = -1;
      index_tempo = strdup("/");
      index_write();
      index_free();
      argument = strdup(newname);
    }
  if (opt_batch)
    {
      // init the core, we do not open it because
      // we don't want any dsp access
      core_init(1);
      printf("Wave written\n");
      // initialize the count dialog
      BpmCountDialog *counter = new BpmCountDialog();
      counter->setBpmBounds(arg_low,arg_high);
      counter->doit();
      counter->finish();
      printf("Counting done\n");
      // finish the core -> remove the raw file
      core_done();
    }
  else if (opt_color)
    {
      core_init(1);
      printf("Wave written\n");
      // initialize the count dialog
      SpectrumDialogLogic *counter = new SpectrumDialogLogic();
      counter->fetchSpectrum();
      counter->finish();
      printf("Fetching Color done\n");
      core_done();
    }
  else
    {
      core_init(0);
      core_open();
      terminal_start();
      core_play();
      terminal_stop();
      core_close();
      core_done();
    }
}
