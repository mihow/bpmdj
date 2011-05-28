/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__players_manager_cpp__
#define __loaded__players_manager_cpp__
using namespace std;
#line 1 "players-manager.c++"
#include "selector.h"
#include "players-manager.h"
#include "scripts.h"
#include "history.h"
#include "bpmdj.h"
#include "info.h"

Song* main_song = NULL;
Song* monitor_song = NULL;
vector<Song*> songs_to_avoid;

void ProcessManager::setMainSong(Song* song)
{
  /** 
   * Here it is difficult to guess in which player the current
   * song is playing. Very likely will we not have succeeded
   * in opening that player. So that player should then be 
   * the chosen one. The challenge here lies in choosing
   * the right empty one. Since we cannot assume anything 
   * useful here. We do not fill in the slot as such.
   */
  ::main_song = song;
  selector->updateProcessView(true);
}

void ProcessManager::process_died(SongSlot* slot)
{
  // was there something on that slot ?
  Song * song = slot->getSong();
  // reread the stopped file
  slot->reread();
  slot->stop();
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
  selector->resetCounter();
  selector->updateProcessView(true);
}

void ProcessManager::start(SongSlot* slot, Song *song)
{
  Song *matchWith=main_song;
  if (!matchWith) matchWith=song;
  Index a(matchWith->get_storedin());
  Index b(song->get_storedin());
  QString playercommand = slot->getPlayCommand(a,b);
  fragmentPlayer.stopOutput();
  fragmentPlayer.waitForStop();
  
  SongProcess* proc=new SongProcess(slot,this);
  QString description="Play " + song->getDisplayTitle() 
    + " at " + slot->getName();
  proc->start(playercommand,description,false);
  slot->start(song,proc);
  
  fragmentPlayer.startOutput();
  fragmentPlayer.waitForStart();
}

bool ProcessManager::startSong(Song *song)
{
  assert(song);
  QString player;
  // if there is still a song playing in the monitor, don't go
  int nextSlot=-1;
  for(int i = 0 ; i< 4 ; i ++)
    {
      if (Config::players[i].canRun())
	{
	  nextSlot=i;
	  break;
	}
    }
  if (nextSlot==-1)
    {
      Error(true,"No free slots to play song\n"
	    "Enable one of the players in the top left panel");
      return false;
    }
  
  start(&Config::players[nextSlot],song);
  // if there is no main song playing:
  // Place it in the main, otherwise, try the monitor
  if (!main_song)
    {
      main_song = song;
      History::this_is_playing(main_song);
      selector->resetCounter();
      selector->updateProcessView(true);
    }
  else if (!monitor_song)
    {
      monitor_song = song;
      selector->updateProcessView(false);
    };
  return true;
}
#endif // __loaded__players_manager_cpp__
