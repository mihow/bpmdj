/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__clock_jack_h__
#define __loaded__clock_jack_h__
using namespace std;
#line 1 "clock-jack.h++"
#ifdef COMPILE_JACK
#ifdef JACK_TRANSPORT
#include "clock-drivers.h"
#include <jack/jack.h>
#include "player-config.h"
#include "types.h"

class clock_jack: public clock_driver
{
  bool sync;
public:
  clock_jack(const PlayerConfig& config);
  virtual void init();
  virtual void shift(signed4);
  virtual void cue_start();
  virtual void tempo_will_change(signed8);
  virtual void request_sync(int retries);
};

void update_first_beat();
void timebase(jack_transport_state_t state, jack_nframes_t nframes, 
	      jack_position_t *pos, int new_pos, void *arg);
void * sync_with_jack(void* neglect);
void become_master(int cond);

#endif
#endif
#endif // __loaded__clock_jack_h__
