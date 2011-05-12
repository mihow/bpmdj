/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__ext_clock_cpp__
#define __loaded__ext_clock_cpp__
using namespace std;
#line 1 "ext-clock.c++"
#ifdef EXT_CLOCK
#include "ext-clock.h"
#include "player-core.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h> 
#include <sched.h>
#include "process.h"
#include "info.h"

ext_clock::ext_clock()
{
  offset=0;
  sync = attached = false;
  slave = 5;
  master = 0;
}

ext_clock::~ext_clock() {
  Debug("ext_clock::~ext_clock()");
  munmap(clock, sizeof(bpmclock));
}

void ext_clock::init()
{
  Debug("ext_clock::init()");
  first_beat=offset=0;

  /* Connection to external clock */
  clock = connect_to_bpmclock();
}

void ext_clock::attach_clock(bool s) {
  if (!attached) {
    sync = s;
    attached = true;
  }
}

void ext_clock::sync_with_clock()
{
  if (attached) {
    int ret;
    ret = sem_trywait(&clock->sem);
    if (ret == 0) {	  
      clock->last_access = clock->currentframe;
      sem_post(&clock->sem);
    }

    if (sync) {
      /* Updating period */
      
      if ((clock->currentperiod == 0) && (::currentperiod.valid())) 
	{ 
	  ret = sem_trywait(&clock->sem);
	  if (ret == 0) {	  
	    clock->currentperiod = ::currentperiod;
	    sem_post(&clock->sem);
	  }
	}
      else if ((!master) && (clock->currentperiod.valid()) && 
	       ((::currentperiod != clock->currentperiod)))
	{
	  if (!master) {
	    clock_driver::changetempo(clock->currentperiod);
	  }
	}
      else if (dsp->get_paused())
	slave = 5;
      else
	sync_now();
    }
  }
}

void ext_clock::sync_now() {
  int ret;

  if (sync) {
    signed8 cp = ::currentperiod.qperiod;
    
    /* Updating position */
    
    signed8 z1 = (::y-dsp->latency()-get_offset()-y_normalise(first_beat)) % cp;
    signed8 z2 = clock->currentframe % cp;
    
    while (abs(z2-z1) > cp / 2) { 
      if (z2  > z1 + cp / 2) 
	{
	  z1 += cp;
	}
      if (z1 > z2 + cp / 2) 
	{
	  z2 += cp;
	}	
    }	   
    
    if (abs(z2-z1) > dsp->latency()) {
      if ((slave) || (!master && clock->master)) {
	::y += (z2-z1);
	while (::y < 0)
	  ::y += cp;
      }
      else {
	ret = sem_trywait(&clock->sem);
	if (ret == 0) {
	  clock->start += (z2-z1);
	  sem_post(&clock->sem);
	}	
      }
    }  
    else if (slave) {
      slave --;
    }
  }
}

void ext_clock::cue_start(cue_info cue)
{
  if (currentperiod > 0) 
    {
      update_first_beat(cue);
      become_slave();
    }
}

void ext_clock::cue_set(cue_info cue)
{
  update_first_beat(cue);
}

void ext_clock::changetempo(signed8 period)
{
  become_master();

  if (sync) {
    int ret;
    do {
      ret = sem_wait(&clock->sem);
      if (ret == 0) {	  
	clock->currentperiod = currentperiod = period;
	sem_post(&clock->sem);
      }
    } while (ret != 0);
  }
  clock_driver::changetempo(period);
  release_master();
}

void ext_clock::update_first_beat(cue_info cue) 
{
  first_beat = cue;
  if (normalperiod > 0) 
    while (first_beat > normalperiod) 
      first_beat -= normalperiod;
}

bpmclock *connect_to_bpmclock()
{
  bpmclock *clock;
  
  if (!ping_bpmclock()) 
    {
      Debug("Starting bpmclock");
      spawn("bpmclock","bpmclock");     
      /* we need to wait for bpmclock to initialize the shared memory */

      sleep(2);
    }

  int shm_fd = shm_open(CLOCK_FILE, O_RDWR, 0);
  
  if ((clock = (bpmclock*) mmap(0, sizeof(bpmclock), (PROT_READ | PROT_WRITE),
				MAP_SHARED, shm_fd, 0)) == MAP_FAILED) 
    {
      Error(true, "bpmclock: Could not map shared memory.\n");
      exit(errno);
    }
  
  return clock;
}

void ext_clock::switch_sync() 
{ 
  if (!sync) 
    {
      become_master();
      clock->currentperiod = currentperiod; 
    }
  sync = !sync;   
  release_master();
}

#endif
#endif // __loaded__ext_clock_cpp__
