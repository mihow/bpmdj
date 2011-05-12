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
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "songplayer.logic.h"
#include "bpm-analyzer.logic.h"
#include "spectrumanalyzer.logic.h"
#include "pattern-analyzer.logic.h"
#include "md5-analyzer.h"
#include "dsp-drivers.h"
#include "index.h"
#include "player-core.h"
#include "scripts.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int   opt_batch = 0;
static int   opt_bpm = 0;
static int   opt_color = 0;
static int   opt_create = 0;
static char* arg_posx = NULL;
static char* arg_posy = NULL;
static int   arg_low = 80;
static int   arg_high = 160;

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
void * go(void* neglect)
{
  app->exec();
  delete(app);
  return neglect;
}

void msg_slowdown(int change)
{
}

void msg_speedup(int change)
{
}

void terminal_start()
{
  pthread_t *y = allocate(1,pthread_t);
  SongPlayerLogic *test=new SongPlayerLogic();
  if (arg_posx && arg_posy)
    test->move(atoi(arg_posx),atoi(arg_posy));
  app->setMainWidget(test);
  test->show();
  pthread_create(y,NULL,go,NULL);
}

void terminal_stop()
{
}

void options_failure(char* err)
{
   // print options
   printf("Usage:  kbpm-play <options> argument\n\n"
          "   -c          --create              create an index file if none exists\n"
	  "   -q          --quiet               be quiet\n"
	  "   -m arg      --match arg           song to match tempo with\n"
	  "   -p nbr nbr  --position nbr nbr    position to place the window\n"
	  "   -v          --verbose             be verbose with respect to latency\n"
	  "   -L nbr      --latency nbr         required latency in ms (default = 744)\n"
#ifdef COMPILE_OSS
	  "  --oss-driver---------------------------------\n"
          "               --oss                 use OSS driver (default)\n"
	  "   -d arg      --dsp arg             dsp device to use (default = /dev/dsp)\n" 
	  //	  "   -x arg      --mixer arg           mixer device to use (default = /dev/mixer)\n"
	  "   -F nbr      --fragments nbr       the number of fragments used to play audio.\n" 
	  "   -X          --nolatencyaccounting does not take into account the latency when marking a cue\n"
#endif 
#ifdef COMPILE_ALSA
	  "  --alsa-driver---------------------------------\n"
          "               --alsa                 use OSS driver (default)\n"
	  "               --dev arg              device to use (default = hw:0,0)\n" 
#endif
	  "  --analysis------------------------------------\n"
	  "   -b          --batch               no ui output, md5sum is automatically checked\n"
	  "               --bpm                 measure bpm\n"
	  //	  "               --pattern             dump pattern\n"
	  "   -l nbr      --low nbr             lowest bpm to look for (default = 120)\n"
	  "   -h nbr      --high nbr            highest bpm to look for (default = 160)\n"
	  "               --spectrum            obtain color at cue-point, no sound, quit imm\n"
	  "   -r arg      --rawpath arg         path that .raw temp files are stored in\n"
	  "   argument                          the index file of the song to handle\n\n%s\n\n",err);
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
	  else if (strcmp(arg,"bpm")==0)
	    opt_bpm=1;
	  //	  else if (strcmp(arg,"pattern")==0)
	  // opt_pattern=1;
	  else if (strcmp(arg,"verbose")==0 ||
		   strcmp(arg,"v")==0)
	    opt_dspverbose=1;
	  else if (strcmp(arg,"latency")==0 ||
		   strcmp(arg,"L")==0)
	    {
	      opt_latency=1;
	      if (++i>=argc)
		options_failure("latency argument scanning error");
	      arg_latency=argv[i];
	    }
#ifdef COMPILE_OSS
	  else if (strcmp(arg,"dsp")==0 ||
		   strcmp(arg,"d")==0)
	    {
	      if (++i>=argc) 
		options_failure("dsp argument scanning error");
	      arg_dsp=argv[i];
	    }
	  else if (strcmp(arg,"fragments")==0 ||
		   strcmp(arg,"F")==0)
	    {
	      opt_ossfragments=1;
	      if (++i>=argc)
		options_failure("fragments argument scanning error");
	      arg_ossfragments = argv[i];
	    }
	  else if (strcmp(arg,"nolatencyaccounting")==0 ||
		   strcmp(arg,"L")==0)
	    opt_oss_nolatencyaccounting=1;
	  else if (strcmp(arg,"oss")==0)
	    {
	      dsp_driver=dsp_oss;
	    }
#endif
#ifdef COMPILE_ALSA
	  else if (strcmp(arg,"dev")==0)
	    {
	      if (++i>=argc) 
		options_failure("dsp argument scanning error");
	      arg_dev=argv[i];
	    }
	  else if (strcmp(arg,"alsa")==0)
	    {
	      dsp_driver=dsp_alsa;
	      if (!opt_latency)
		arg_latency = "150";
	    }
#endif
	  else if (strcmp(arg,"create")==0 ||
		   strcmp(arg,"c")==0)
	    opt_create=1;
	  else if (strcmp(arg,"spectrum")==0)
	    opt_color=1;
	  else if (strcmp(arg,"rawpath")==0 ||
		   strcmp(arg,"r")==0)
	    {
	      if (++i>=argc) 
		options_failure("rawpath argument scanning error");
	      arg_rawpath=argv[i];
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
	  /*	  else if (strcmp(arg,"mixer")==0 ||
		   strcmp(arg,"x")==0)
	    {
	      if (++i>=argc)
		options_failure("mixer argument scanning error");
	      arg_mixer=argv[i];
	    }
	  */
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
  if ((opt_color || opt_bpm) && !opt_batch)
    {
      options_failure("to start an analyzer, you need to supply the --batch option");
    }
}

void show_error(int err, int err2, const char*text)
{
  if (err==err2)
    {
      const QString a=QString("Error");
      const QString b=QString(text);
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      exit(err);
    }
}

void normal_start()
{
  int err;
  err = core_init(0);
  show_error(err, err_needidx, "Please enter the index file, not the "SONG_EXT" file\nAn index file can be made with 'kbpm-play -c'\n");
  show_error(err, err_noraw, "No raw file to be read. Probably the .mp3 is broken.\n");
  show_error(err, err_nospawn, "Unable to spawn decoding process.\nPlease check your PATH environment variable\n");

  err = core_open();
  show_error(err, err_dsp, "Unable to open dsp device\n");
  //  show_error(err, err_mixer, "Unable to open mixer device\n");
  
  terminal_start();
  core_play();
  terminal_stop();
  core_close();
  core_done();
}

void batch_start()
{
  int nr=0;
  // 0. core init: synchronous without opening dsp
  core_init(1);
  printf("%d. Wave written: %s\n",nr++,Index::index->readable_description());
  // 1. md5sum
  if (!Index::index->index_md5sum || strcmp(Index::index->index_md5sum,"")==0)
    {
      Md5Analyzer * md5_analyzer = new Md5Analyzer();
      md5_analyzer->run();
      printf("%d. Md5 sum: %s\n",nr++,Index::index->index_md5sum);
    }
  // 2. bpm
  if (opt_bpm)
    {
      BpmAnalyzerDialog *counter = new BpmAnalyzerDialog();
      counter->setBpmBounds(arg_low,arg_high);
      counter->run();
      counter->finish();
      printf("%d. Bpm count: %s\n",nr++,Index::index->get_tempo_str());
    }
  // 3. spectrum
  if (opt_color)
    {
      SpectrumDialogLogic *counter = new SpectrumDialogLogic();
      counter->fetchSpectrum();
      printf("%d. Spectrum\n",nr++);
    }
  // 4. pattern
  /*  if (opt_pattern)
    {
      PatternAnalyzerLogic *pattern = new PatternAnalyzerLogic(false);
      pattern->run();
      printf("%d. Pattern\n",nr++);
    }
  */
  // 99. finish the core -> remove the raw file
  core_done();
}

int main(int argc, char *argv[])
{
  process_options(argc,argv);
  // create an application
  app = new QApplication(argc,argv);
  // if we need to create an index file we'll make it.
  if (opt_create)
    {
      char newname[500];
      strcpy(newname,argument);
      int len = strlen(newname);
      if (len>=4 && newname[len-4]=='.')
	strcpy(newname+len-3,"idx");
      else
	options_failure("Sorry, song must end on either " SONG_EXT);
      // create index and write it..
      Index *index = new Index();
      index->index_file = strdup(argument);
      index->index_tags=strdup("New");
      index->set_period(-1);
      index->index_changed = 1;
      index->write_idx(newname);
      delete index;
      argument = strdup(newname);
    }
  
  if (opt_batch)
    batch_start();
  else
    normal_start();
}
