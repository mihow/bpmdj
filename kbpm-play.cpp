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
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <qradiobutton.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "songplayer.logic.h"
#include "energy-analyzer.h"
#include "bpm-analyzer.logic.h"
#include "spectrumanalyzer.logic.h"
#include "pattern-analyzer.logic.h"
#include "md5-analyzer.h"
#include "dsp-drivers.h"
#include "dsp-oss.h"
#include "dsp-alsa.h"
#include "dsp-none.h"
#include "dsp-mixed.h"
#include "memory.h"
#include "player-core.h"
#include "scripts.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
static int    opt_batch = 0;
static int    opt_bpm = 0;
static int    arg_bpm = 0;
static int    opt_color = 0;
static int    opt_create = 0;
static int    opt_energy = 0;
static int    arg_posx = -1;
static int    arg_posy = -1;
static int    arg_low = 80;
static int    arg_high = 160;
static bool   opt_remote = false;
static char * arg_remote = "";

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
SongPlayerLogic *player_window;

void * go(void* neglect)
{
  app->exec();
  delete(player_window);
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
  player_window = new SongPlayerLogic();
  if (arg_posx >=0 && arg_posy >=0)
     player_window->move(arg_posx,arg_posy);
  app->setMainWidget(player_window);
  player_window->show();
  pthread_create(y,NULL,go,NULL);
}

void terminal_stop()
{
}

void options_failure(char* err)
{
  QMessageBox::critical(NULL,
			"Command line arguments",
			QString(err),
			QMessageBox::Ok,QMessageBox::NoButton);
  
  // print options
  printf("Usage:  kbpm-play <options> argument\n\n"
	 "   -c          --create              create an index file if none exists\n"
	 "   -q          --quiet               be quiet\n"
	 "   -m arg      --match arg           song to match tempo with\n"
	 "   -p nbr nbr  --position nbr nbr    position to place the window\n"
	 "               --rms float           noramlize to rms value\n"
	 "               --remote user@host    copy necessary files (requires ssh & scp)\n"
	 OSS_OPTION_HELP
	 NONE_OPTION_HELP
	 MIXED_OPTION_HELP
	 ALSA_OPTION_HELP
	 "  --analysis------------------------------------\n"
	 "   -b          --batch               no ui output, md5sum is automatically checked\n"
	 "               --bpm [1,2,3,4]       measure bpm with specified technique (default = 1)\n"
	 //	  "               --pattern             dump pattern\n"
	 "   -l nbr      --low nbr             lowest bpm to look for (default = 120)\n"
	 "   -h nbr      --high nbr            highest bpm to look for (default = 160)\n"
	 "               --spectrum            obtain color at cue-point, no sound, quit imm\n"
	 "   -e          --energy              measure energy levels\n"
	 "   -r arg      --rawpath arg         path that .raw temp files are stored in\n"
	 "   argument                          the index file of the song to handle\n\n%s\n\n",err);
  exit(1);
}

#define next_arg if (++i>=argc) options_failure("missing argument to last option");
#define arg_int(name) next_arg name=atoi(argv[i]);
#define arg_str(name) next_arg name=argv[i];
#define arg_float(name) next_arg name=atof(argv[i]);

void process_options(int argc, char* argv[])
{
  int i = 1 ;
  int used = 0;
  bool dsp_chosen = false;
  dsp = new dsp_none();
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
	  
	  // check option
	  if (dsp && (used = dsp->parse_option(arg, argv[i+1])))
	    {
	      i+=used-1;
	    }
	  else if (option(arg,"quiet","q"))
	    {
	      opt_quiet = true;
	    }
	  else if (option(arg,"batch","b"))
	    {
	      dsp_chosen = true;
	      opt_batch=true;
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
	  else if (option(arg,"rms"))
	    {
	      opt_rms=true;
	      arg_float(arg_rms);
	    } 
	  else if (option(arg,"none"))
	    {
	      dsp = new dsp_none();
	      dsp_chosen = true;
	    }
	  else if (option(arg,"remote"))
	    {
	      opt_remote=true;
	      arg_str(arg_remote);
	      argv[i]=argv[i-1]="";
	    } 
	  else if (option(arg,"mixed"))
	    {
	      arg_int(dsp_mixed::mix_dev);
	      dsp = new dsp_mixed();
	      dsp_chosen = true;
	    }
#ifdef COMPILE_OSS
	  else if (option(arg,"oss"))
	    {
	      dsp = new dsp_oss();
	      dsp_chosen = true;
	    }
#endif
#ifdef COMPILE_ALSA
	  else if (option(arg,"alsa"))
	    {
	      dsp = new dsp_alsa();
	      dsp_chosen = true;
	    }
#endif
	  else if (option(arg,"create","c"))
	    opt_create=true;
	  else if (option(arg,"spectrum"))
	    opt_color = true;
	  else if (option(arg,"rawpath","r"))
	    {
	      arg_str(arg_rawpath);
	    }
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
		  sprintf(err,"version mismatch\nkbpm-play is version "VERSION
			  "\nkbpm-dj is version %s",argv[i]);
		  options_failure(err);
		  exit(1);
		}
	      exit(0);
	    } 
	  else if (option(arg,"match","m"))
	    { 
	      opt_match=true;
	      arg_str(arg_match); 
	    } 
	  else if (option(arg,"position","p"))
	    {
	      arg_int(arg_posx); 
	      arg_int(arg_posy); 
	    }
	}
      else
	argument = argv[i];
    }
   if (argument==NULL)
     options_failure("requires at least one argument");
   if (!dsp_chosen)
     options_failure("please select a dsp device\n");
   if (opt_bpm)
     {
	if (arg_bpm<1) 
	  options_failure("selected bpm analyzing technique too low (starts with 1)\n");
	if (arg_bpm>4)
	  options_failure("selected bpm analyzing technique too hi (ends with 4)\n");
     }
   if ((opt_color || opt_bpm || opt_energy) && !opt_batch)
     options_failure("to start an analyzer, you need to supply the --batch option");
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

void normal_start()
{
  int err;
  err = core_init(0);
  if (show_error(err, err_needidx, "Please enter the index file, not the "SONG_EXT" file\nAn index file can be made with 'kbpm-play -c'\n")
      || show_error(err, err_noraw, "No raw file to be read. Probably the .mp3 is broken.\n")
      || show_error(err, err_nospawn, "Unable to spawn decoding process.\nPlease check your PATH environment variable\n"))
    exit(err);
  
  err = core_open();
  if (show_error(err, err_dsp, "Unable to open dsp device\n"))
    {
      core_done();
      exit(err);
    }
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
  printf("%d. Wave written: %s\n",nr++,playing->readable_description());
  // 1. md5sum
  if (!playing->get_md5sum() || strcmp(playing->get_md5sum(),"")==0)
    {
      Md5Analyzer * md5_analyzer = new Md5Analyzer();
      md5_analyzer->run();
      printf("%d. Md5 sum: %s\n",nr++,playing->get_md5sum());
    }
  // 2. energy levels
  if (!playing->fully_defined_energy() || opt_energy)
    {
      EnergyAnalyzer * energy_analyzer = new EnergyAnalyzer();
      energy_analyzer->run();
      printf("%d. Min, max : (L: %d, R: %d), (L: %d, R: %d)\n",nr++,
	     playing->get_min().left,
	     playing->get_min().right,
	     playing->get_max().left,
	     playing->get_max().right);
      printf("%d. Mean, RMS : (L: %d, R: %d), (L: %g, R: %g)\n",nr++,
	     playing->get_mean().left,
	     playing->get_mean().right,
	     playing->get_power().left,
	     playing->get_power().right);
    }
  // 3. bpm
  if (opt_bpm)
    {
      BpmAnalyzerDialog *counter = new BpmAnalyzerDialog();
      counter->setBpmBounds(arg_low,arg_high);
      if (arg_bpm==2) counter->ultraLongFFT->setChecked(true);
      else if (arg_bpm==3) counter->enveloppeSpectrum->setChecked(true);
      else if (arg_bpm==4) counter->fullAutoCorrelation->setChecked(true);
      counter->run();
      counter->finish();
      printf("%d. Bpm count: %s\n",nr++,playing->get_tempo().get_charstr());
    }
  // 4. spectrum
  if (opt_color)
    {
      SpectrumDialogLogic *counter = new SpectrumDialogLogic();
      counter->fetchSpectrum();
      printf("%d. Spectrum\n",nr++);
    }
  // 5. pattern
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

// The escaping necessary for ssh to work is ridicoulous..
// An ordinary containement within " ... " does not work, so we must
// escape every character on its own. Especially the & is important
// because this spawns a background procses :)
char * escape(char * in)
{
  char escaped[2048];
  int c;
  int i=0;
  while((c=*(in++)) && i < 2048)
    {
      if (c=='\n' || c=='&' || c=='\\' || c==' ' || c=='(' || c==')' || c=='\'')
	{
	  escaped[i++]='\\';
	}
      escaped[i++]=c;
    }
  assert(i<2048);
  escaped[i]=0;
  return strdup(escaped);
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
  Index * toplay = new Index(argument);
  printf("[-> data   ------------------------------------]\n");
  char mp3[2000];
  sprintf(mp3,"./music/%s",toplay->get_filename());
  if (!to_remote(mp3)) return 1;
  if (!to_remote(argument)) return 2;
  if (opt_match && strcmp(arg_match,argument)) if (!to_remote(arg_match)) return 3;
  char newcmd[2000];
  sprintf(newcmd," ssh -X %s ",arg_remote);
  for(int i = 0 ; i < argc ; i ++)
    {
      strcat(newcmd,escape(escape(argv[i])));
      strcat(newcmd," ");
    }
  printf("[* execute ------------------------------------]\n");
  if (!execute(newcmd)) return 4;
  printf("[<- data   ------------------------------------]\n");
  if (!from_remote(argument)) return 5;
  if (!delete_remote(argument)) return 6;
  if (!delete_remote(mp3)) return 7;
  if (opt_match && strcmp(arg_match,argument)) if (!delete_remote(arg_match)) return 8;
  return 0;
}

int main(int argc, char *argv[])
{
  app = new QApplication(argc,argv);
  process_options(argc,argv);
  if (opt_remote)
    return remote(argc,argv);
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
      index->set_filename(strdup(argument));
      index->set_tags(strdup("New"));
      index->set_period(-1);
      index->set_changed();
      index->write_idx(newname);
      delete(index);
      argument = strdup(newname);
    }
  
  if (opt_batch)
    batch_start();
  else
    normal_start();
}
