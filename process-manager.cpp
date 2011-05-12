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
QSong* ProcessManager::playing_songs[4] = {NULL,NULL,NULL,NULL};

// when a process stops this method is called
// normally it will immidatelly invoke the one and
// only process manager.

void ToSwitchOrNotToSwitchSignal(int sig,siginfo_t *info, void* hu)
{
  for(int i = 0 ; i < 4 ; i ++)
    if (processManager->died_pids[i]==-1)
      {
	processManager->died_pids[i] = info->si_pid;
	return;
      }
  assert(0);
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
  mainTempo=0;
  for(int i =0; i <4;i++)
    {
      died_pids[i]=-1;
      player_pids[i]=0;
      playing_songs[i]=NULL;
    }
  // catch signals
  struct sigaction *act;
  act=(struct sigaction*)malloc(sizeof(struct sigaction));
  assert(act);
  act->sa_sigaction=ToSwitchOrNotToSwitchSignal;
  act->sa_flags=SA_SIGINFO;
  sigaction(SIGUSR1,act,NULL);
  // ignore sigchlds
  signal(SIGCHLD,SIG_IGN);
};

void ProcessManager::setMainSong(QSong* song)
{
  playing_songs[0]=song;
  mainTempo=atof((const char*)song->song_tempo);
  selector->updateProcessView();
}

void ProcessManager::processDied(int pid)
{
  if (pid==player_pids[1])
    clearPlayer(1);
  else if (pid==player_pids[0])
    switchMonitorToMain();
  else if (pid==player_pids[2])
    clearPlayer(2);
  else if (pid==player_pids[3])
    clearPlayer(3);
  else 
    assert(0);
}

void ProcessManager::checkSignals()
{
  for(int i = 0 ; i < 4 ; i ++)
    {
      int tmp;
      if ((tmp=died_pids[i])!=-1)
	{
	  died_pids[i]=-1;
	  processDied(tmp);
	}
    }
}

void ProcessManager::clearPlayer(int id, bool update)
{
  if(!playing_songs[id])
    return;
  // reread the song that was playing
  playing_songs[id]->reread();
  // WVB - this should be done automatically
  selector->parseTags(playing_songs[id]->song_tags);
  player_pids[id] = 0;
  playing_songs[id]=NULL;
  if (update)
    selector->updateProcessView();
}

void ProcessManager::switchMonitorToMain()
{
  // clear the playing song
  clearPlayer(0,false);
  // set the new tempo
  mainTempo = monitorTempo;
  // clear the monitor
  monitorTempo = 0;
  player_pids[0]=player_pids[1];
  player_pids[1]=0;
  // the song is already playing, now use the next command next time */
  monitorPlayCommand=(monitorPlayCommand == 1 ? 2 : 1);
  playing_songs[0]=playing_songs[1];
  playing_songs[1]=NULL;
  // write playing sucker to disk
  if (playing_songs[0])
    {
      Played::Play(playing_songs[0]->song_index);
      playing_songs[0]->song_played=true;
    }
  selector->resetCounter();
  selector->updateProcessView();
}

void ProcessManager::startExtraSong(int id, QSong *song)
{
  char playercommand[500];
  // create suitable start command
  QSong *matchWith=playingInMain();
  if (!matchWith) 
    matchWith=song;
  playing_songs[id]=song;
  sprintf(playercommand, 
	  (const char*)(id == 3 ? Config::playCommand3
			: Config::playCommand4),
	  (const char*)matchWith->song_index, 
	  (const char*)song->song_index);
  // fork the command and once the player exists immediatelly stop
  if (!(player_pids[id]=fork()))
    { 
      system(playercommand);
      kill(getppid(),SIGUSR1);
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
  if (player_pids[1]!=0)
    {
      const QString a=QString("Error");
      const QString b=QString("Cannot start playing in monitor, other monitor song still playing !");
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      return;
    }
  // update monitor fields
  monitorTempo=atof((const char*)song->song_tempo);
  // create suitable start command
  playing_songs[1]=song;
  matchWith=playingInMain();
  if (!matchWith) matchWith=playing_songs[1];
  player = monitorPlayCommand == 1 ? Config::playCommand1 : Config::playCommand2;
  sprintf(playercommand, (const char*)player, (const char*)matchWith->song_index, (const char*)playing_songs[1]->song_index);
  // fork the command and once the player exists immediatelly stop
  if (!(player_pids[1]=fork()))
    { 
      system(playercommand);
      kill(getppid(),SIGUSR1);
      exit(0);
    }
  // if there is no main song playing. Place it main, otherwise, try the monitor
  if (!playingInMain())
    switchMonitorToMain();
  // update the process view of course
  selector->updateProcessView();
}
