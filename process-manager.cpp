/****
 BpmDj: Free Dj Tools
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
using namespace std;
#line 1 "process-manager.c++"
#include "songselector.logic.h"
#include "process-manager.h"
#include "scripts.h"
#include "history.h"
#include "kbpm-dj.h"

Song* main_song = NULL;
Song* monitor_song = NULL;

ProcessManager::ProcessManager(SongSelectorLogic * sel) :
  BasicProcessManager(4)
{
  listener = sel;
  selector = sel;
};

void ProcessManager::setMainSong(Song* song)
{
  /** 
   * Here it is difficult to guess in which player the current
   * song is playing. Very likely will we not have succeeded
   * in opening that player. So that player should then be 
   * the choosen one. The challenge here lies in choosing
   * tjhe right empty one. Since we cannot assume anything 
   * useful here. We do not fill in the slot as such.
   */
  ::main_song = song;
  get_selector()->updateProcessView(true);
}

void ProcessManager::clearId(int id)
{
  // was there something on that slot ?
  Song * song = Config::players[id].getSong();
  if(!song) return;
  // reread the stopped file
  Config::players[id].reread();
  Config::players[id].stop();
  // we clear the active pid
  BasicProcessManager::clearId(id);
  // was the song the main_song ???
  if (main_song == song)
    switchMonitorToMain();
  else if (monitor_song == song)
    monitor_song = NULL;
}

void ProcessManager::switchMonitorToMain()
{
  main_song = monitor_song;
  monitor_song = NULL;
  History::this_is_playing(main_song);
  get_selector()->resetCounter();
  get_selector()->updateProcessView(true);
}

void ProcessManager::start(int id, Song *song)
{
  Song *matchWith=main_song;
  if (!matchWith) matchWith=song;
  Index a(matchWith->get_storedin());
  Index b(song->get_storedin());
  QString playercommand = Config::players[id].getPlayCommand(a,b);
  fragmentPlayer.stopOutput();
  fragmentPlayer.waitForStop();
  BasicProcessManager::start(id, playercommand,
			     Config::players[id].getLogName(),
			     false);
  Config::players[id].start(song);
  fragmentPlayer.startOutput();
  fragmentPlayer.waitForStart();
}

#ifdef INCOMPLETE_FEATURES
void ProcessManager::startExtraSong(int id, Song *song)
{
  start(id,song);
  get_selector()->updateProcessView(false);
}
#endif

bool ProcessManager::startSong(Song *song)
{
  assert(song);
  QString player;
  // if there is still a song playing in the monitor, don't go
#ifdef INCOMPLETE_FEATURES
  if (::monitor_song)
    {
      Error(true,"Cannot start playing in monitor, other monitor song still playing !");
      return false;
    }
#endif
  int nextSlot=-1;
  for(int i = 0 ; i< 4 ; i ++)
    {
      // !active_pids[i] && 
      if (Config::players[i].canRun())
	{
	  nextSlot=i;
	  break;
	}
    }
  if (nextSlot==-1)
    {
      Error(true,"No free slots to play song");
      return false;
    }

  start(nextSlot,song);
  // if there is no main song playing:
  // Place it in the main, otherwise, try the monitor
  if (!main_song)
    {
      main_song = song;
      History::this_is_playing(main_song);
      get_selector()->resetCounter();
      get_selector()->updateProcessView(true);
    }
  else if (!monitor_song)
    {
      monitor_song = song;
      get_selector()->updateProcessView(false);
    };
  return true;
}
