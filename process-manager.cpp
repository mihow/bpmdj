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

#include <unistd.h>
#include <dirent.h>
#include <qmessagebox.h>
#include <ctype.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include "preferences.h"
#include "about.h"
#include "tagbox.h"
#include "songselector.logic.h"
#include "kbpm-played.h"
#include "version.h"
#include "kbpm-dj.h"
#include "process-manager.h"
#include "qsong.h"

ProcessManager* processManager = NULL;

double ProcessManager::mainTempo = 0;
QSong*  ProcessManager::playingInMain;
QSong*  ProcessManager::playingInMonitor;

// when a process stops this method is called
// normally it will immidatelly invoke the one and
// only process manager.

void ToSwitchOrNotToSwitchSignal(int sig,siginfo_t *info, void* hu)
{
  if (processManager->died_pid1==-1)
    {
      processManager->died_pid1 = info->si_pid;
      return;
    }
  if (processManager->died_pid2==-1)
    processManager->died_pid2 = info->si_pid;
}

ProcessManager::ProcessManager(SongSelectorLogic * sel)
{
  // the process manager is a singleton
  assert(processManager==NULL);
  assert(sel);
  processManager = this;
  selector = sel;
  // initialise fields
  monitorPlayCommand=1;
  playingInMain=NULL;
  playingInMonitor=NULL;
  mainTempo=0;
  died_pid1=-1;
  died_pid2=-1;
  // catch signals
  struct sigaction *act;
  act=(struct sigaction*)malloc(sizeof(struct sigaction));
  assert(act);
  act->sa_sigaction=ToSwitchOrNotToSwitchSignal;
  act->sa_flags=SA_SIGINFO;
  sigaction(SIGUSR1,act,NULL);
  monitorpid=0;
  // ignore sigchlds
  signal(SIGCHLD,SIG_IGN);
};

void ProcessManager::setMainSong(QSong* s)
{
  playingInMain=s;
}

void ProcessManager::clearMonitor()
{
  if (!playingInMonitor) 
    return;
  // reread the song that was playing
  playingInMonitor->reread();
  // TOFIX -- normally when a song is updated 
  //          the songviewitem should be updated 
  // immediatelly...
  selector->parseTags(playingInMonitor->song_tags);
  monitorpid = 0;
  playingInMonitor = NULL;
  selector->updateProcessView();
}

void ProcessManager::processDied(int pid)
{
  // uiteindelijk moeten we de monitor switchen
  if (pid!=monitorpid)
    switchMonitorToMain();
  else
    clearMonitor();
}

void ProcessManager::checkSignals()
{
  int tmp;
  if ((tmp=died_pid1)!=-1)
    {
      died_pid1=-1;
      processDied(tmp);
    }
  if ((tmp=died_pid2)!=-1)
    {
      died_pid2=-1;
      processDied(tmp);
    }
}

void ProcessManager::switchMonitorToMain()
{
  // set the new tempo
  mainTempo = monitorTempo;
  // clear the monitor
  monitorTempo = 0;
  monitorpid = 0;
  // reread the main when necessary
  if (playingInMain)
    {
      playingInMain->reread();
      // TOFIX -- song should do this automatically
      selector->parseTags(playingInMain->song_tags);
    }
  // the song is already playing, now use the next command next time */
  monitorPlayCommand=(monitorPlayCommand == 1 ? 2 : 1);
  playingInMain=playingInMonitor;
  playingInMonitor=NULL;
  // write playing sucker to disk
  if (playingInMain)
    {
      Played::Play(playingInMain->song_index);
      playingInMain->song_played=true;
    }
  selector->resetCounter();
  selector->updateProcessView();
}

void ProcessManager::startSongPlayer3(QSong *song)
{
  char playercommand[500];
  // create suitable start command
  QSong *matchWith=playingInMain;
  if (!matchWith) 
    matchWith=song;
  sprintf(playercommand, 
	  (const char*)Config::playCommand3, 
	  (const char*)matchWith->song_index, 
	  (const char*)song->song_index);
  // fork the command and once the player exists immediatelly stop
  if (!fork())
    { 
      system(playercommand);
      exit(0);
    }
  selector->updateProcessView();
}

void ProcessManager::startSong(QSong *song)
{
  assert(song);
  QString player;
  QSong *matchWith;
  char playercommand[500];
  // if there is still a song playing in the monitor, don't go
  if (monitorpid!=0)
    {
      const QString a=QString("Error");
      const QString b=QString("Cannot start playing in monitor, other monitor song still playing !");
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      return;
    }
  // update monitor fields
  monitorTempo=atof((const char*)song->song_tempo);
  // create suitable start command
  playingInMonitor=song;
  matchWith=playingInMain;
  if (!matchWith) matchWith=playingInMonitor;
  player = monitorPlayCommand == 1 ? Config::playCommand1 : Config::playCommand2;
  sprintf(playercommand, (const char*)player, (const char*)matchWith->song_index, (const char*)playingInMonitor->song_index);
  // fork the command and once the player exists immediatelly stop
  if (!(monitorpid=fork()))
    { 
      system(playercommand);
      kill(getppid(),SIGUSR1);
      exit(0);
    }
  // update the process view of course
  selector->updateProcessView();
}
