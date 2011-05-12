/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__clock_drivers_h__
#define __loaded__clock_drivers_h__
using namespace std;
#line 1 "clock-drivers.h++"
#include "types.h"
#include "period-type.h"
#include "player-core.h"

/**
 * This variable denotes the target period, which is the length of 1 measure 
 * (=4 beats) at the command line requested playing speed
 */
extern quad_period_type targetperiod;

/**
 * This variable denotes the current period, which is the length of 1 measure 
 * (=4 beats) which is used to play the song. The tempo must be set through
 * the changetempo() function.
 */
extern quad_period_type currentperiod;

/**
 * This variable denotes the period of 1 measure (4 beats)
 * in the native song. This is thus the reciprocal of the native tempo
 * To modify the tempo, one should go through the metronome.
 */
extern quad_period_type normalperiod;


/**
 * x is the data position in the raw file (thus at normal tempo)
 * This is calculated during streaming. So updating this variable 
 * doesn't have much effect. To have some effect you might need
 * to set the y variable properly.
 */
extern signed8 x;

/**
 * y is the position in the playing file (thus at target tempo)
 * So if a song is 4 minutes and we play it at 50% of the normal speed
 * then we have a total length of 8 minutes, which means that the position
 * halfway the song will be at samplerate*4 and the last position in the song
 * is at samplerate*8. If the song would play at double the speed then the
 * last position in the song is at samplerate*4/8 (or samplerate/2)
 * In other words. this is a tricky variable which is afterward transformed
 * to the proper playing position ::x during the streaming. The advantage
 * of this setup is that y can simply be incremented.
 */
extern signed8 y;

/**
 * converts the y position (samples given the current tempo) to x
 * (playing position in the raw file)
 */
signed8 x_normalise(signed8 y);

/**
 * converts the x position (samples in the raw file) to a position given 
 * the current tempo.
 */
signed8 y_normalise(signed8 x);

class clock_driver
{
protected:
  signed4 offset;
  bool verbose;
public:
  clock_driver()
  { 
    verbose = true;
  };
  /**
   * Initialize the driver by allocating necessary
   * threads and data structures.
   */
  virtual void init()
  {
  };

  /**
   * This is called regularly to sync with the external clock.
   *
   */
  virtual void sync_with_clock() {};

  /**
   * The clock driver is already running but the user
   * started from a specific cue again
   */
  virtual void cue_start(cue_info cue) {};
  virtual void cue_set(cue_info cue) {};

  virtual void shift(signed4 direction)
  {
    ::y+=direction;
    if (::y<0) ::y=0;
    offset+=direction;
  }

  /*
   * will modify the tempo and inform he current metronome
   */
  
  virtual void changetempo(signed8 period);

  /**
   * This function is called by the routines that can detect or modify the
   * native tempo. When overriding this function, you should call the super class
   * Beware: this method does not update the playing file and can be
   * used for instance by the jack clock driver. The global
   * set_normalperiod on the other hand will update the index file; which 
   * might not be wanted.
   */
  virtual void set_normalperiod(quad_period_type newnormalperiod)
  {
    assert(newnormalperiod>0);
    if (currentperiod==normalperiod)
      currentperiod=normalperiod=newnormalperiod;
    else
      {
	normalperiod=newnormalperiod;
	::y = ::x * currentperiod / normalperiod;
      }
  }

  signed4 get_offset() { return offset; }
  virtual void start_sync() {};
  virtual void stop_sync() {};
  virtual void attach_clock(bool s) {};
  virtual void detach_clock() {};
  virtual void switch_sync() {};
  virtual void sync_now() {};
  virtual void become_master() { };
  virtual void become_slave() { };
  virtual void release_master() { };
};

extern clock_driver *metronome;

#endif // __loaded__clock_drivers_h__
