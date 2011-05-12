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
#line 1 "fragment-player.c++"
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
  if (!playing || !dsp || finished || stopped) return Done;
  static const int check_every = 44100/3;
  for(int i = check_every; i ; i--)
    {
      dsp->write(playing->samples[curpos++]);
      curpos%=playing->size;
      if (curpos==0) 
	{
	  finished=true;
	  return Done;
	}
    }
  return RevisitAfterIncoming;
};

elementResult ActiveFragmentPlayer::playWave(Fragment fragment)
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
      dsp->open();
      dsp->start();
      just_opened=true;
    }
  if (!dsp) return Done;
  playing = fragment;
  curpos = 0;
  finished = false;
  // this pause will drop any pending samples, and continue
  // immediatelly since our wait_for_unpause returns immediatelly.
  if (!just_opened) dsp->pause();
  if (curpos==0) queue_playChunk(++expected_playchunk);
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
      dsp->close(false);
      delete dsp;
      dsp = NULL;
      playing = Fragment();
      curpos = 0;
      finished = false;
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

/**
 * The DSP Drivers can be paused and will then call wait_for_unpause
 * which should return when the pause has finished.
 * In our case we dont't want to use the pause capabilities. So 
 * we return immediatelly
 */
signed8 x;
quad_period_type normalperiod;
void wait_for_unpause()
{
}

FragmentPlayer fragmentPlayer;
