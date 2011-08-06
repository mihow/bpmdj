/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__fragment_player_cpp__
#define __loaded__fragment_player_cpp__
using namespace std;
#include "player-core.h"
#include "fragment-cache.h"
#include "fragment-player.h"
#include "player-config.h"
#include "dsp-drivers.h"
#include "dsp-none.h"

elementResult ActiveFragmentPlayer::playWave(FragmentInMemory fragment)
{
  if (stopped) return Done;
  checkValidDsp();
  delivery.reset(fragment);  
  if (!dsp)
    {
      SongSlot *free_slot = NULL;
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
      if (is_none_driver(dsp))
	{
	  delete dsp;
	  dsp=NULL;
	}
      else if (dsp->open(false)!=err_none)
	{
	  delete dsp;
	  dsp=NULL;
	}
      else
	dsp->start(&delivery);
    }
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

elementResult ActiveFragmentPlayer::delivererFinished()
{
  closeDsp();
  return Done;
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
  while(!object.stopped) ;
}

void FragmentPlayer::waitForStart()
{
  while(object.stopped) ;
}
#endif // __loaded__fragment_player_cpp__
