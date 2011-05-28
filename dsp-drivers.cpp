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
#ifndef __loaded__dsp_drivers_cpp__
#define __loaded__dsp_drivers_cpp__
using namespace std;
#line 1 "dsp-drivers.c++"
#include <pthread.h>
#include <assert.h>
#include <qdialog.h>
#include <qstring.h>
#include "player-config.h"
#include "dsp-oss.h"
#include "dsp-alsa.h"
#include "dsp-none.h"
#include "dsp-jack.h"
#include "scripts.h"
#include "player-core.h"
#include "info.h"

#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#ifndef COMPILE_JACK
#error -------------------------------------------
#error You should at least compile one dsp driver.
#error Use -D COMPILE_OSS or -D COMPILE_ALSA or
#error or -D COMPILE_JACK
#error -------------------------------------------
#endif
#endif
#endif

dsp_driver::dsp_driver(const PlayerConfig & config) 
{ 
  stopped = true;
  stop_request = false;
  paused = true;
  starting=false;
  playrate=0;
  verbose = config.get_dsp_verbose();
  playrate=config.get_dsp_playrate();
};

void dsp_driver::init()
{
}

dsp_driver * dsp_driver::get_driver(PlayerConfig * cfg)
{
  switch (cfg->get_player_dsp())
    {
    case 0 : return new dsp_none ( * cfg ) ;
    case 1 :
#ifdef COMPILE_OSS
      return new dsp_oss  ( * cfg ) ;
#else
      Warning("The OSS driver was not compiled in");
      return new dsp_none( * cfg );
#endif
    case 2 :
#ifdef COMPILE_ALSA
      return new dsp_alsa ( * cfg ) ;
#else
      Warning("The ALSA driver was not compiled in");
      return new dsp_none( * cfg );
#endif
    case 3 : assert(0); 
    case 4 : 
#ifdef COMPILE_JACK
      return new dsp_jack ( * cfg ) ;
#else
      Warning("The Jack driver was not compiled in");
      return new dsp_none( * cfg );
#endif
    }
  return new dsp_none( * cfg );
}

static void * go2(void* d)
{
  dsp_driver* dsp=(dsp_driver*)d;
  assert(dsp);
  dsp->run_pusher_thread();
  return NULL;
}

void dsp_driver::run_pusher_thread()
{
  stop_request=false;
  stopped=false;
#ifndef NO_AUTOSTART
  paused=false;
#else
  paused=true;
#endif
  starting=false;
  while(!stop_request)
    {
      if (paused) 
	{
	  internal_pause();
	  wait_for_unpause();
	  internal_unpause();
	}
      write(audio->read());
    }
  stopped=true;
}

void dsp_driver::start(audio_source* from)
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_driver::start()");
#endif
  assert(from);
  audio=from;
  pthread_t *y = bpmdj_allocate(1,pthread_t);
  starting=true;
  pthread_create(y,NULL,go2,(void*)this);
  while(starting) usleep(10);
}

void dsp_driver::stop()
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_driver_stop(): entered");
#endif
  if (!stopped)
    {
      stop_request=true;
      // enables the release of the internal transfer thread
      unpause();
      // wait until the stop is understood by the producer
      while (!stopped) usleep(10);
    }
  stop_request=false;
  // close the dsp device immediately
  close(true);
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_driver_stop(): finished");
#endif
}

void dsp_driver::pause()
{
  paused = true;
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_driver::pause()");
#endif
}

void dsp_driver::unpause()
{
  paused = false;
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_driver::unpause()");
#endif
}

/**
 * This function is called from within the dsp driver itself and only when 
 * using the synchronous pusher. It should return only when the pause got 
 * unpaused.
 */
void dsp_driver::wait_for_unpause()
{
#ifdef DEBUG_WAIT_STATES
  Debug("wait_for_unpause(): entered");
  fflush(stdout);
#endif
  while(paused) usleep(10);
#ifdef DEBUG_WAIT_STATES
  Debug("wait_for_unpause(): finished");
  fflush(stdout);
#endif
}

dsp_driver *dsp = NULL;
#endif // __loaded__dsp_drivers_cpp__
