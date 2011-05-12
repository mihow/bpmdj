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
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include "preferences.h"
#include "about.h"
#include "songselector.logic.h"
#include "history.h"
#include "version.h"
#include "kbpm-dj.h"
#include "process-manager.h"
#include "qsong.h"
#include "config.h"
#include "scripts.h"
#include "memory.h"

Song* *ProcessManager::playing_songs = NULL;

// when a process stops this method is called
// normally it will immidatelly invoke the one and
// only process manager.

ProcessManager::ProcessManager(SongSelectorLogic * sel) :
  BasicProcessManager(4,sel)
{
  monitorPlayCommand=1;
  selector = sel;
  playing_songs = allocate(pid_count,Song*);
  for(int i = 0; i < pid_count; i++)
    playing_songs[i]=NULL;
};

void ProcessManager::setMainSong(Song* song)
{
  song->realize();
  playing_songs[0]=song;
  get_selector()->updateProcessView(true);
}

void ProcessManager::clearId(int id)
{
  if (id == 0)
    switchMonitorToMain();
  else
    clearPlayer(id,true);
}

void ProcessManager::clearPlayer(int id, bool update)
{
  if(!playing_songs[id]) return;
  playing_songs[id]->reread();
  active_pids[id] = 0;
  playing_songs[id]=NULL;
  if (update)
    get_selector()->updateProcessView(id==0);
}

void ProcessManager::switchMonitorToMain()
{
  // clear the playing song
  clearPlayer(0,false);
  // clear the monitor
  active_pids[0]=active_pids[1];
  active_pids[1]=0;
  // the song is already playing, now use the next command next time
  monitorPlayCommand=(monitorPlayCommand == 1 ? 2 : 1);
  playing_songs[0]=playing_songs[1];
  playing_songs[1]=NULL;
  // write playing sucker to disk
  Played::Play(playing_songs[0]);
  get_selector()->resetCounter();
  get_selector()->updateProcessView(true);
}

void ProcessManager::startExtraSong(int id, Song *song)
{
  char playercommand[500];
  // create suitable start command
  song->realize();
  Song *matchWith=playingInMain();
  if (!matchWith) 
    matchWith=song;
  playing_songs[id]=song;
  sprintf(playercommand,
	  (const char*)(id == 3 ? Config::get_playCommand4()
			: Config::get_playCommand3()),
	  (const char*)Config::get_tmp_directory(),
	  (const char*)matchWith->get_storedin(), 
	  (const char*)song->get_storedin());
  start(id,playercommand);
  get_selector()->updateProcessView(false);
}

void ProcessManager::startSong(Song *song)
{
  assert(song);
  QString player;
  song->realize();
  Song *matchWith;
  char playercommand[500];
  // if there is still a song playing in the monitor, don't go
  if (active_pids[1]!=0)
    {
      const QString a=QString("Error");
      const QString b=QString("Cannot start playing in monitor, other monitor song still playing !");
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      return;
    }
  // create suitable start command
  playing_songs[1]=song;
  matchWith=playingInMain();
  if (!matchWith) matchWith=playing_songs[1];
  player = monitorPlayCommand == 1 ? Config::get_playCommand1() : Config::get_playCommand2();
  sprintf(playercommand, 
	  (const char*)player, 
	  (const char*)Config::get_tmp_directory(),
	  (const char*)matchWith->get_storedin(), 
	  (const char*)playing_songs[1]->get_storedin());
  // fork the command and once the player exists immediatelly stop
  start(1,playercommand);
  // if there is no main song playing. Place it in the main, otherwise, try the monitor
  if (!playingInMain())
    {
      switchMonitorToMain();
      return;
    }
  // update the process view of course
  get_selector()->updateProcessView(false);
}
