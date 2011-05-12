/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__fragment_player_cpp__
#define __loaded__fragment_player_cpp__
using namespace std;
#line 1 "fragment-player.c++"
#include "player-core.h"
#include "fragment-cache.h"
#include "fragment-player.h"
#include "player-config.h"
#include "dsp-drivers.h"

elementResult ActiveFragmentPlayer::playChunk(int t)
{
  // we only want the most recent playchunk, which 
  // in our situation is known by its expected timestamp
  if (t!=expected_playchunk) return Done;
  checkValidDsp();
  if (!delivery.playing || !dsp || delivery.finished || stopped) return Done;
  usleep(333);
  return RevisitAfterIncoming;
};

elementResult ActiveFragmentPlayer::playWave(FragmentInMemory fragment)
{
  if (stopped) return Done;
  checkValidDsp();
  bool just_opened = false;
  if (!dsp)
    {
      SongProcess *free_slot = NULL;
      for(int i = 0 ; i < 4 ; i++)
	if (Config::players[i].canRun())
	  {
	    free_slot=&Config::players[i];
	    player_slot=i;
	    break;
	  }
      if (!free_slot)
	return Done;
      PlayerConfig player_config(free_slot->getName());
      dsp = dsp_driver::get_driver(&player_config);
      if (dsp->open(false)!=err_none)
	{
	  delete dsp;
	  dsp=NULL;
	}
      else
	{
	  dsp->start(&delivery);
	  just_opened=true;
	}
    }
  if (!dsp) return Done;
  delivery.reset(fragment);
  
  // this pause will drop any pending samples, and continue
  // immediatelly since our wait_for_unpause returns immediatelly.
  //if (!just_opened) dsp->pause();
  queue_playChunk(++expected_playchunk);
  return Done;
};

void ActiveFragmentPlayer::checkValidDsp()
{
  if (player_slot>=0 && dsp) 
    if (!Config::players[player_slot].canRun())
      closeDsp();
}

elementResult ActiveFragmentPlayer::terminate()
{
  closeDsp();
  deactivate();
  return Done;
}

void ActiveFragmentPlayer::closeDsp()
{
  if (dsp)
    {
      dsp->stop();
      delete dsp;
      dsp = NULL;
      delivery.reset(FragmentInMemory());
    }
  player_slot = -1;
}

elementResult ActiveFragmentPlayer::stopOutput()
{
  closeDsp();
  stopped = true;
  return Done;
}

elementResult ActiveFragmentPlayer::startOutput()
{
  stopped = false;
  return Done;
}

void FragmentPlayer::waitForStop()
{
  while(object.dsp || !object.stopped) ;
}

void FragmentPlayer::waitForStart()
{
  while(object.stopped) ;
}

FragmentPlayer fragmentPlayer;
#endif // __loaded__fragment_player_cpp__
