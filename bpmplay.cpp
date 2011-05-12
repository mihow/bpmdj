/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __loaded__bpmplay_cpp__
#define __loaded__bpmplay_cpp__
using namespace std;
#line 1 "bpmplay.c++"
#include <Qt/qapplication.h>
#include <Qt/qmessagebox.h>
#include <Qt/qlistview.h>
#include <Qt/qlcdnumber.h>
#include <Qt/q3header.h>
#include <Qt/qgroupbox.h>
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
#include "rythm-analyzer.h"
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

/*-------------------------------------------
 *         Templates we need
 *-------------------------------------------*/

// types
template class smallhistogram_type<32>;
template class smallhistogram_type<96>;
template class histogram_property<32>;
template class histogram_property<96>;

// signals
template double normalize_abs_max<double>(double*, long);
template double find_abs_max<double>(double*, long);
template class Fft<2>;
template class Signal<double, 2>;
template class Signal<short, 2>;
template class BasicSignal<double, 2>;
template class BasicSignal<short, 2>;
template class HalfComplex<double, 2>;

template class Sample<double, 2>;
template class Sample<short,  2>;
template       Sample<short,  2>::Sample(Sample<double, 2>);
template       Sample<double, 2>::Sample(Sample<short, 2>);

template class Shift<short, 2>;
template class Shift<double,2>;

template void BasicSignal<double, 2>::add<short>(BasicSignal<short, 2> const&);
template void BasicSignal<double, 2>::add<double>(BasicSignal<double, 2> const&);
template BasicSignal<double, 2>& BasicSignal<double, 2>::operator-=<double>(BasicSignal<double, 2> const&);
template BasicSignal<short, 2>& BasicSignal<short, 2>::operator-=<short>(BasicSignal<short, 2> const&);
template Signal<double, 2>::Signal(BasicSignal<short, 2> const&);
template Signal<double, 2>::Signal(BasicSignal<double, 2> const&);
template class Haar<double, double, 2>;
template class Haar<short, double, 2>;

template class SignalIO<signed2,2>;
template void SignalIO<short, 2>::writeSamples(BasicSignal<double, 2> const&, unsigned4);
template void SignalIO<short, 2>::readSamples(BasicSignal<double, 2>&, unsigned4);
template void SignalIO<short, 2>::writeSamples(BasicSignal<signed2, 2> const&, unsigned4);
template void SignalIO<short, 2>::readSamples(BasicSignal<signed2, 2>&, unsigned4);

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int    opt_batch = 0;
static int    opt_bpm = 0;
static int    arg_bpm = 0;
static int    opt_color = 0;
static int    opt_create = 0;
static int    opt_energy = 0;
static int    opt_rythm = 0;
static int    arg_low = 80;
static int    arg_high = 160;
static bool   opt_remote = false;
static char * arg_remote = "";
       bool   opt_setup = false;
       char * arg_config = "";
       bool   opt_check = false;
PlayerConfig* config;

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
SongPlayer *player_window;

void options_failure(char* err)
{
  QMessageBox::critical(NULL,
			"Command line arguments",
			QString(err),
			QMessageBox::Ok,QMessageBox::NoButton); 
 
  // print options
  printf("Usage:  bpmplay <options> argument\n\n"
	 "               --config name         name of the configfile\n"
	 "   -s          --setup               setup the configuration without opening dsp/playing song\n"
	 "   -c          --create              create an index file if none exists\n"
	 "   -q          --quiet               be quiet\n"
	 "   -m arg      --match arg           song to match tempo with\n"
	 "               --remote user@host    copy necessary files (requires ssh & scp)\n"
	 "  --analysis------------------------------------\n"
	 "   -b          --batch               no ui output, md5sum is automatically checked\n"
	 "               --bpm [1,2,3,4,5]     measure bpm with specified technique (default = 1)\n"
	 "   -l nbr      --low nbr             lowest bpm to look for (default = 120)\n"
	 "   -h nbr      --high nbr            highest bpm to look for (default = 160)\n"
	 "               --spectrum            obtain color & echo information\n"
	 "               --rythm               obtain rythm information\n"
	 "   -e          --energy              measure energy levels\n"
	 "   argument                          the index file of the song to handle\n\n%s\n\n",err);
  _exit(1);
}

#define next_arg if (++i>=argc) options_failure("missing argument to last option");
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
	  if (option(arg,"quiet","q"))
	    {
	      opt_quiet = true;
	    }
	  else if (option(arg,"batch","b"))
	    {
	      opt_batch=true;
	    }
	  else if (option(arg,"setup","s"))
	    {
	      opt_setup=true;
	    }
	  else if (option(arg,"energy","e"))
	    {
	      opt_energy=true;
	    } 
	  else if (option(arg,"bpm"))
	    {
	      opt_bpm=true; 
	      arg_int(arg_bpm);
	    }
	  else if (option(arg,"remote"))
	    {
	      opt_remote=true;
	      arg_str(arg_remote);
	      argv[i]=argv[i-1]="";
	    } 
	  else if (option(arg,"config"))
	    {
	      opt_config=true;
	      arg_str(arg_config);
	    } 
	  else if (option(arg,"create","c"))
	    opt_create=true;
	  else if (option(arg,"spectrum"))
	    opt_color = true;
	  else if (option(arg,"rythm"))
	    opt_rythm = true;
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
		  sprintf(err,"version mismatch\nbpmplay (%s) is version "VERSION
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
	  if (dsp->is_none())
	    {
	      if (!opt_check)
		QMessageBox::information(NULL, "DSP Driver Selection",
					 "No DSP driver has been selected. Go to the\n"
					 "options tab and select an appropriate one.",
					 QMessageBox::Ok,QMessageBox::NoButton);
	    }
	}
      
      // initialize the dsp now
      if (argument==NULL)
	options_failure("the program requires that at least one\n.idx file is passed as a command line argument");
      if (opt_bpm)
	{
	  if (arg_bpm<1) 
	    options_failure("selected bpm analyzing technique too low (starts with 1)\n");
	  if (arg_bpm>5)
	    options_failure("selected bpm analyzing technique too hi (ends with 5)\n");
	}
      if ((opt_color || opt_bpm || opt_energy) && !opt_batch)
	options_failure("to start an analyzer, you need to supply the --batch option");
    }
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

/**
 * The main application thread must be the window thread. This is very annoying 
 * but that's the way it is. So, we create the application, create the window
 * and then initialize the core. This cannot be done in this function
 * this should be done when the applicaion is initialized and the event
 * loop running (otherwise we cannot give error boxes). Therefore we
 * post an event to the player_window which will be received as soon
 * as the event loop starts running 
 */
void setup_start()
{
  playing = NULL;
  check_capacities();
  player_window = new SongPlayer();
  //app->setMainWidget(player_window);
  player_window->tab->setCurrentPage(TAB_OPTIONS);
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
  player_window = new SongPlayer();
  //app->setMainWidget(player_window);
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
      const QString b=QString("The enabled capacities include at least one capacity\n"
			      "that cannot be found on this machine (")+QString(arg_config)+QString(")");
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      _exit(10);
    }
  player_window = new SongPlayer();
  //app->setMainWidget(player_window);
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
  // 0. core init: synchronous without opening dsp
  core_meta_init();
  core_object_init(true);
  Info("%d. Wave written: %s",nr++,(const char*)playing->readable_description());
  // 1. md5sum
  if (playing->get_md5sum().isEmpty())
    {
      Md5Analyzer * md5_analyzer = new Md5Analyzer();
      md5_analyzer->start();
      Info("%d. Md5 sum: %s",nr++,(const char*)playing->get_md5sum());
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
  // 3. bpm
  if (opt_bpm)
    {
      if (playing->get_time_in_seconds()<760)
	{
	  BpmAnalyzerDialog *counter = new BpmAnalyzerDialog();
	  counter->setBpmBounds(arg_low,arg_high);
	  if (arg_bpm==2) counter->ultraLongFFT->setChecked(true);
	  else if (arg_bpm==3) counter->enveloppeSpectrum->setChecked(true);
	  else if (arg_bpm==4) counter->fullAutoCorrelation->setChecked(true);
	  else if (arg_bpm==5) counter->weightedEnvCor->setChecked(true);
	  counter->start();
	  Info("%d. Bpm count: %s",nr++,playing->get_tempo().get_charstr());
	}
      else
	{
	  Info("%d. Bpm count skipped because song is longer than 760s",nr++);
	}
    }
  // 4. spectrum
  if (opt_color)
    {
      SpectrumAnalyzer *counter = new SpectrumAnalyzer();
      counter->start();
      Info("%d. Spectrum / Echo",nr++);
    }
  // 5. rythm
  if (opt_rythm)
    {
      if (playing->get_time_in_seconds()<760)
	{
	  RythmAnalyzer *r = new RythmAnalyzer();
	  r->start();
	  Info("%d. Rythm",nr++);
	}
      else
	{
	  Info("%d. Bpm count skipped because song is longer than 760s",nr++);
	}
    }
  // 99. finish the core -> remove the raw file
  core_done();
}

bool to_remote(char* filename)
{
  return vexecute(" ssh %s  mkdir -p %s",arg_remote,escape(escape(dirname(strdup(filename)))))
    && vexecute(" scp -q %s %s:%s",escape(filename),arg_remote,escape(escape(dirname(strdup(filename)))));
}

bool from_remote(char* filename)
{
  return vexecute(" scp -q %s:%s %s ",arg_remote,escape(escape(filename)),escape(filename));
}

bool delete_remote(char* filename)
{
  return vexecute(" ssh %s rm %s",arg_remote,escape(escape(filename)));
}

int remote(int argc, char* argv[])
{
  char mp3[2000];
  if (!opt_setup)
    {
      Index * toplay = new Index(argument);
      Remote("Uploading song & index file");
      sprintf(mp3,"./music/%s",(const char*)toplay->get_filename());
      if (!to_remote(mp3)) return 1;
      if (!to_remote(argument)) return 2;
      if (opt_match && strcmp(arg_match,argument)) if (!to_remote(arg_match)) return 3;
    }
  char newcmd[2000];
  sprintf(newcmd," ssh -X %s ",arg_remote);
  for(int i = 0 ; i < argc ; i ++)
    {
      strcat(newcmd,escape(escape(argv[i])));
      strcat(newcmd," ");
    }
  Remote("Executing remote player");
  if (!execute(newcmd)) return 4;
  if (!opt_setup)
    {
      Remote("Downloading index file");
      if (!from_remote(argument)) return 5;
      if (!delete_remote(argument)) return 6;
      if (!delete_remote(mp3)) return 7;
      if (opt_match && strcmp(arg_match,argument)) if (!delete_remote(arg_match)) return 8;
    }
  return 0;
}

int main(int argc, char *argv[])
{
  init_embedded_files();
  app = new QApplication(argc,argv);
  process_options(argc,argv);
  if (opt_remote)
    _exit(remote(argc,argv));
  if (opt_create)
    {
      Index *index = createNewIndexFor(argument,"./");
      argument = strdup(index->get_storedin());
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
