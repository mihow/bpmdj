/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__ext_clock_h__
#define __loaded__ext_clock_h__
using namespace std;
#line 1 "ext-clock.h++"
#ifdef EXT_CLOCK
#include "clock-drivers.h"
#include "player-config.h"
#include "bpmclock.h"

class ext_clock: public clock_driver
{
public:
  signed8 first_beat;
  bpmclock *clock;
  bool sync, attached;
  unsigned4 slave;
  unsigned4 master;

  ext_clock();
  ~ext_clock();
  virtual void init();
  virtual void sync_with_clock();
  virtual void cue_start(cue_info cue);
  virtual void cue_set(cue_info cue);
  virtual void changetempo(signed8 period);
  virtual void start_sync() { sync = true; };
  virtual void stop_sync() { sync = false; };
  virtual void attach_clock(bool s);
  virtual void detach_clock() { sync = attached = false; };
  virtual void sync_now();
  virtual void switch_sync();
  virtual void become_master() { master++; clock->master = true; slave = 0; };
  virtual void release_master() { 
    if (master) {
      master--; 
      if (!master)
	clock->master = false; 
    }
  };
  virtual void become_slave() { slave = 5; };
  void update_first_beat(cue_info cue);
};

bpmclock *connect_to_bpmclock();
#endif
#endif // __loaded__ext_clock_h__
