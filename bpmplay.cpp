/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__bpmplay_cpp__
#define __loaded__bpmplay_cpp__
using namespace std;
#line 1 "bpmplay.c++"
#include <Qt/qapplication.h>
#include <Qt/qmessagebox.h>
#include <Qt/qlistview.h>
#include <Qt/qlcdnumber.h>
#include <Qt/qgroupbox.h>
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <Qt/qradiobutton.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <Qt/qtabwidget.h>
#include <math.h>
#include <pthread.h>
#include "player.h"
#include "energy-analyzer.h"
#include "bpm-analyzer.h"
#include "spectrum-analyzer.h"
#include "rhythm-analyzer.h"
#include "md5-analyzer.h"
#include "dsp-drivers.h"
#include "dsp-oss.h"
#include "dsp-alsa.h"
#include "dsp-none.h"
#include "memory.h"
#include "player-core.h"
#include "scripts.h"
#include "signals.h"
#include "histogram-property.h"
#include "smallhistogram-type.h"
#include "bpmplay.h"
#include "capacity-checker.h"
#include "bpmplay-event.h"
#include "clock-drivers.h"
#include "hues.h"
#include "info.h"

/*-------------------------------------------
 *         Templates we need
 *-------------------------------------------*/
template class smallhistogram_type<32>;
template class smallhistogram_type<96>;

template class histogram_property<32>;
template class histogram_property<96>;
template float8 normalize_abs_max<float8>(float8*, long);
template float8 find_abs_max<float8>(float8*, long);
template class Fft<2>;
template class Signal<float8, 2>;
template class Signal<short, 2>;
template class BasicSignal<float8, 2>;
template class BasicSignal<short, 2>;
template class HalfComplex<float8, 2>;

template class Sample<float8, 2>;
template class Sample<short,  2>;
template       Sample<short,  2>::Sample(Sample<float8, 2>);
template       Sample<float8, 2>::Sample(Sample<short, 2>);

template class Shift<short, 2>;
template class Shift<float8,2>;

template void BasicSignal<float8, 2>::
add<short>(BasicSignal<short, 2> const&);

template void BasicSignal<float8, 2>::
add<float8>(BasicSignal<float8, 2> const&);

template BasicSignal<float8, 2>& BasicSignal<float8, 2>::
operator-=<float8>(BasicSignal<float8, 2> const&);

template BasicSignal<short, 2>& BasicSignal<short, 2>::
operator-=<short>(BasicSignal<short, 2> const&);

template Signal<float8, 2>::
Signal(BasicSignal<short, 2> const&);

template Signal<float8, 2>::
Signal(BasicSignal<float8, 2> const&);

template class Haar<float8, float8, 2>;
template class Haar<short, float8, 2>;

template class SignalIO<signed2,2>;
template void SignalIO<short, 2>::
writeSamples(BasicSignal<float8, 2> const&, unsigned4);

template void SignalIO<short, 2>::
readSamples(BasicSignal<float8, 2>&, unsigned4);

template void SignalIO<short, 2>::
writeSamples(BasicSignal<signed2, 2> const&, unsigned4);

template void SignalIO<short, 2>::
readSamples(BasicSignal<signed2, 2>&, unsigned4);

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int    opt_batch = 0;
static int    opt_bpm = 0;
static int    arg_bpm = 0;
static int    opt_color = 0;
static int    opt_create = 0;
static int    opt_energy = 0;
static int    opt_rhythm = 0;
static int    arg_low = 80;
static int    arg_high = 160;
static bool   opt_remote = false;
static bool   opt_copymusic = false;
static const char * arg_remote = "";
       bool   opt_setup = false;
       const char * arg_config = "";
       bool   opt_check = false;
PlayerConfig* config;

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
Player *player_window;

void options_failure(const char* err)
{
  QMessageBox::critical(NULL,
			"Command line arguments",
			QString(err),
			QMessageBox::Ok,QMessageBox::NoButton); 
  printf("Usage:  bpmplay <options> argument\n\n"
"        --config name     name of the configfile\n"
" -s     --setup           setup a configuration\n"
" -c     --create          create an index file if none exists\n"
" -q     --quiet           be quiet\n"
" -m arg --match arg       song to match tempo with\n"
"--remote------------------------------------------\n"
"      --remote user@host  executes bpmplay remotely\n"
"      --copymusic         copy music first\n"
"--analysis----------------------------------------\n"
" -b     --batch           no ui output, md5sum is automatically checked\n"
"        --bpm [1,2,3,4,5] measure bpm with specified technique (default = 1)\n"
" -l nbr --low nbr         lowest bpm to look for (default = 120)\n"
" -h nbr --high nbr        highest bpm to look for (default = 160)\n"
"        --spectrum        obtain color & echo information\n"
"        --rhythm          obtain rhythm information\n"
" -e     --energy          measure energy levels\n"
" argument                 the index file of the song to handle\n\n%s\n\n",err);
  _exit(1);
}

#define next_arg if (++i>=argc) \
options_failure("missing argument to last option");
#define arg_int(name) next_arg name=atoi(argv[i]);
#define arg_str(name) next_arg name=argv[i];
#define arg_float(name) next_arg name=atof(argv[i]);

void process_options(int argc, char* argv[])
{
  int i = 1 ;
  bool opt_config = false;
  for(i = 1 ; i < argc ; i ++)
    {
      if (argv[i][0]=='-')
	{
	  char* arg = NULL;
	  // check long opt ?
	  if (argv[i][1]=='-')
	    arg=argv[i]+2;
	  else if (argv[i][1]==0 || argv[i][2]!=0)
	    options_failure("option neither short or long");
	  else arg=argv[i]+1;
	  
	  // check option
	  if (option(arg,"quiet","q"))       opt_quiet = true;
	  else if (option(arg,"batch","b"))  opt_batch=true;
	  else if (option(arg,"setup","s"))  opt_setup=true;
	  else if (option(arg,"energy","e")) opt_energy=true;
	  else if (option(arg,"bpm"))
	    {
	      opt_bpm=true; 
	      arg_int(arg_bpm);
	    }
	  else if (option(arg,"remote"))
	    {
	      opt_remote=true;
	      arg_str(arg_remote);
	      argv[i]=argv[i-1]=(char*)"";
	    }
	  else if (option(arg,"copymusic")) opt_copymusic=true;
	  else if (option(arg,"config"))
	    {
	      opt_config=true;
	      arg_str(arg_config);
	    } 
	  else if (option(arg,"create","c")) opt_create = true;
	  else if (option(arg,"spectrum"))   opt_color = true;
	  else if (option(arg,"rhythm"))     opt_rhythm = true;
	  else if (option(arg,"low","l"))
	    {
	      arg_int(arg_low);
	    }
	  else if (option(arg,"high","h"))
	    {
	      arg_int(arg_high);
	    }
	  else if (option(arg,"check-version"))
	    {
	      next_arg;
	      if (strcmp(argv[i],VERSION)!=0)
		{
		  char err[5000];
		  sprintf(err,"version mismatch\nbpmplay (%s) is version "
			  VERSION
			  "\nbpmdj is version %s",arg_config,argv[i]);
		  options_failure(err);
		  _exit(1);
		}
	      opt_check=true;
	    }
	  else if (option(arg,"match","m"))
	    { 
	      opt_match=true;
	      arg_str(arg_match);
	    }
	}
      else
	argument = argv[i];
    }

  if (opt_remote)
    {
      // when working remote we don't initialize the
      // local configuration file
      dsp = new dsp_none(*config);
    }
  else
    {
      // initialize the local configuration file if mentioned
      if (opt_config)
	config = new PlayerConfig(arg_config);
      else
	config = new PlayerConfig();
      
      // are we setting up the show ?
      if (opt_setup)
	{
	dsp = new dsp_none(*config);
	return;
	}
      // are we planning to analyze ?
      if (opt_batch)
	dsp = new dsp_none(*config);
      else
	{
	  dsp = dsp_driver::get_driver(config);
	  if (is_none_driver() && !opt_check)
	      QMessageBox::information(NULL, "DSP Driver Selection",
	       "No DSP driver has been selected. Go to the\n"
	       "options tab and select an appropriate one.",
	       QMessageBox::Ok,QMessageBox::NoButton);
	}

      // we always need a clock driver also for the analyzers.
      // During analysis we might set the tempo with set_normal_period.
      metronome = new clock_driver();
      
      // initialize the DSP now
      if (argument==NULL)
	options_failure("the program requires that at least one\n"
			".idx file is passed as a command line argument");
      if (opt_bpm)
	{
	  if (arg_bpm<1) 
	    options_failure("selected BPM analyzing technique too low (starts"
			    " with 1)\n");
	  if (arg_bpm>5)
	    options_failure("selected BPM analyzing technique too hi (ends"
			    " with 5)\n");
	}
      if ((opt_color || opt_bpm || opt_energy) && !opt_batch)
	options_failure("to start an analyzer, you need to supply the "
			"--batch option");
    }
}

bool show_error(int err, int err2, QString text)
{
  if (err==err2)
    {
      Error(true,text.toAscii().data());
      return true;
    }
  return false;
}

/**
 * The main application thread must be the window thread. This is very annoying 
 * but that's the way it is. So, we create the application, create the window
 * and then initialize the core. This cannot be done in this function
 * this should be done when the application is initialized and the event
 * loop running (otherwise we cannot give error boxes). Therefore we
 * post an event to the player_window which will be received as soon
 * as the event loop starts running 
 */
void setup_start()
{
  playing = NULL;
  check_capacities();
  player_window = new Player();
  player_window->tab->setCurrentIndex(TAB_OPTIONS);
  player_window->show();
  config->load_ui_position(player_window);
  app->postEvent(player_window,new InitAndStart());
  app->exec();
  config->save_ui_position(player_window);
  delete(player_window);
}

void normal_start()
{
  int err = core_meta_init();
  if (show_error(err, err_needidx, 
		 "Please enter the index file, not the "+extensionList()+
		 " file\nAn index file can be made with 'bpmplay -c'\n"
		 "The argument you passed was "+QString(argument)))
    _exit(err);
  player_window = new Player();
  player_window->show();
  config->load_ui_position(player_window);
  app->postEvent(player_window,new InitAndStart());
  app->exec();
  config->save_ui_position(player_window);
  delete(player_window);
}

/**
 * When checking the player we automatically check 
 * which players are available and check this with the player
 * configuration...
 */
void check_start()
{
  int err = core_meta_init();
  assert(err!=err_needidx);
  check_capacities();
  // do we have unavailable capacities that are enabled (!disabled)
  if (unavailable_capacities & ~config->get_disabled_capacities())
    {
      const QString a=QString("Mismatched capacities");
      const QString b=QString("The enabled capacities include at least one "
	"capacity\nthat cannot be found on this machine (")
	+QString(arg_config)+QString(")");
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      _exit(10);
    }
  player_window = new Player();
  player_window->showMinimized();
  app->postEvent(player_window,new InitAndStart());
  app->exec();
  err = player_window->result() == QDialog::Rejected;
  delete(player_window);
  if (err)
    _exit(10); 
}

void batch_start()
{
  int nr=0;
  // 0. core init: synchronous without opening DSP
  core_meta_init();
  core_object_init(true);
  if (!playing) 
    {
      Debug("No valid index file given to analyze");
      return;
    }
  Info("%d. Wave written: %s",nr++,
       playing->readable_description().toAscii().data());
  // 1. md5sum
  if (playing->get_md5sum().isEmpty())
    {
      Md5Analyzer * md5_analyzer = new Md5Analyzer();
      md5_analyzer->start();
      Info("%d. Md5 sum: %s",nr++,
	   playing->get_md5sum().toAscii().data());
    }
  // 2. energy levels
  if (!playing->fully_defined_energy() || opt_energy)
    {
      EnergyAnalyzer * energy_analyzer = new EnergyAnalyzer();
      energy_analyzer->start();
      Info("%d. Min, max : (L: %d, R: %d), (L: %d, R: %d)",nr++,
	   (int)playing->get_min().left,
	   (int)playing->get_min().right,
	   (int)playing->get_max().left,
	   (int)playing->get_max().right);
      Info("%d. Mean, RMS : (L: %d, R: %d), (L: %g, R: %g)",nr++,
	   (int)playing->get_mean().left,
	   (int)playing->get_mean().right,
	   playing->get_power().left,
	   playing->get_power().right);
    }
  // 3. BPM
  if (opt_bpm)
    {
      if (playing->get_time_in_seconds()>=760)
	Info("%d. Bpm count skipped because song is longer than 760s",nr++);
      else if (playing->get_min()==playing->get_max())
	Info("%d. Bpm count skipped because song is empty",nr++);
      else
	{
	  BpmAnalyzerDialog *counter = new BpmAnalyzerDialog();
	  counter->setBpmBounds(arg_low,arg_high);
	  if (arg_bpm==2) counter->ultraLongFFT->setChecked(true);
	  else if (arg_bpm==3) counter->enveloppeSpectrum->setChecked(true);
	  else if (arg_bpm==4) counter->fullAutoCorrelation->setChecked(true);
	  else if (arg_bpm==5) counter->weightedEnvCor->setChecked(true);
	  counter->start();
	  Info("%d. Bpm count: %s",nr++,
	       playing->get_tempo().qstring().toAscii().data());
	}
    }
  
  // 4. spectrum
  if (opt_color)
    {
      Info("%d. Spectrum / Echo",nr++);
      SpectrumAnalyzer *counter = new SpectrumAnalyzer();
      counter->start();
    }
  // 5. rhythm
  if (opt_rhythm)
    {
      if (playing->get_time_in_seconds()>=760)
	Info("%d. Rhythm analysis skipped because song is longer than 760s",
	     nr++);
      else if (!playing->get_spectrum())
	Info("%d. Need spectrum before analyzing rhythm",nr++);
      else 
	{
	  RhythmAnalyzer *r = new RhythmAnalyzer();
	  r->start();
	  Info("%d. Rhythm",nr++);
	}
    }
  
  // 98. Setting tags as necessary (Bug #195)
  if (playing->get_min()==playing->get_max())
    {
      Info("%d. Tagging song as Empty because of no sensible energy levels",
	   nr++);
      playing->set_tags(playing->get_tags()+" Empty");
    }
  if (playing->get_time_in_seconds()>=760)
    {
      Info("%d. Tagging song as TooLongToAnalyze because of its length",nr++);
      playing->set_tags(playing->get_tags()+" TooLongToAnalyze");
    }
  if (playing->get_time_in_seconds()>=30*60)
    {
      Info("%d. Tagging song as Long since it is longer than 30'",nr++);
      playing->set_tags(playing->get_tags()+" Long");
    }
  if (playing->get_time_in_seconds()<30)
    {
      Info("%d. Tagging song as Short since it is shorter than 30\"",nr++);
      playing->set_tags(playing->get_tags()+" Short");
    }

  // 99. finish the core -> remove the raw file
  core_done();
}

bool to_remote(char* filename)
{
  return vexecute(SSH" %s mkdir -p %s",arg_remote,
		  escape(escape(dirname(strdup(filename)))))
    && vexecute(" scp -q %s %s:%s",escape(filename),arg_remote,
		escape(escape(dirname(strdup(filename)))));
}

bool from_remote(char* filename)
{
  return vexecute(" scp -q %s:%s %s ",arg_remote,
		  escape(escape(filename)),escape(filename));
}

bool delete_remote(char* filename)
{
  return vexecute(SSH" %s rm %s",arg_remote,escape(escape(filename)));
}

int remote(int argc, char* argv[])
{
  char mp3[2000];
  if (!opt_setup)
    {
      Index * toplay = new Index(argument);
      if (opt_copymusic)
	Remote("Uploading song & index file");
      else
	Remote("Uploading index file");
      sprintf(mp3,"./music/%s",
	      toplay->get_filename().toAscii().data());
      if (opt_copymusic)
	if (!to_remote(mp3)) return 1;
      if (!to_remote(argument)) return 2;
      if (opt_match && strcmp(arg_match,argument))
	if (!to_remote(arg_match)) return 3;
    }
  char newcmd[2000];
  sprintf(newcmd,SSH" %s ",arg_remote);
  for(int i = 0 ; i < argc ; i ++)
    {
      strcat(newcmd,escape(escape(argv[i])));
      strcat(newcmd," ");
    }
  Remote("Executing remote player");
  if (!execute("Executing remote player",newcmd)) return 4;
  if (!opt_setup)
    {
      Remote("Downloading index file");
      if (!from_remote(argument)) return 5;
      if (!delete_remote(argument)) return 6;
      if (opt_copymusic)
	if (!delete_remote(mp3)) return 7;
      if (opt_match && strcmp(arg_match,argument))
	if (!delete_remote(arg_match)) return 8;
    }
  return 0;
}

int main(int argc, char *argv[])
{
  init_embedded_files();
  dsp_driver::init();
  app = new QApplication(argc,argv);
  init_hues();
  process_options(argc,argv);
  if (opt_remote)
    _exit(remote(argc,argv));
  if (opt_create)
    {
      Index *index = createNewIndexFor(argument,"./");
      argument = strdup(index->get_storedin().toAscii().data());
      delete(index);
    }
  if (opt_check)
    check_start();
  else if (opt_batch)
    batch_start();
  else if (opt_setup)
    setup_start();
  else
    normal_start();
  delete(app);
  _exit(0);
}
#endif // __loaded__bpmplay_cpp__
