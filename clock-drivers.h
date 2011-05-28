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
#ifndef __loaded__clock_drivers_h__
#define __loaded__clock_drivers_h__
using namespace std;
#line 1 "clock-drivers.h++"
#include "period-type.h"
#include "player-core.h"

/**
 * This variable denotes the target period, which is the length of 1 measure 
 * (=4 beats) at the command line requested playing speed
 */
extern quad_period_type targetperiod_metarate;

/**
 * This variable denotes the current period, which is the length of 1 measure 
 * (=4 beats) used to play the song at the current playrate. 
 * The tempo must be set through. So it is possible to have a song with a 
 * normalperiod of 100, but it should be played at a currentperiod of 50
 * (hence double as fast).
 */
extern quad_period_type currentperiod_metarate;

/**
 * This variable denotes the period of 1 measure (4 beats)
 * in the native song. This is thus the reciprocal of the native tempo
 * To modify the tempo, one should go through the metronome.
 */
extern quad_period_type normalperiod_metarate;


/**
 * x is the data position in the raw file (thus at normal tempo and at 
 * diskrate Hz). This is calculated during streaming. So updating this 
 * variable doesn't have much effect. To have some effect you might need
 * to set the y variable properly.
 */
extern signed8 x_diskrate;

/**
 * y is the position in the playing file (thus at target tempo and at playrate)
 * So if a song is 4 minutes and we play it at 50% of the normal speed
 * then we have a total length of 8 minutes, which means that the position
 * halfway the song will be at samplerate*4 and the last position in the song
 * is at samplerate*8. If the song would play at double the speed then the
 * last position in the song is at samplerate*4/8 (or samplerate/2)
 * In other words. this is a tricky variable which is afterward transformed
 * to the proper playing position ::x during the streaming. The advantage
 * of this setup is that y can simply be incremented.
 */
extern signed8 y_playrate;

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
public:
  clock_driver()
  { 
  };
  /**
   * Initialize the driver by allocating necessary
   * threads and data structures.
   */
  virtual void init()
  {
  };

  void shift_metarate(signed4 direction_metarate)
  {
    shift_playrate(metarate_to_playrate(direction_metarate));
  }
  virtual void shift_playrate(signed4 direction_playrate)
  {
    ::y_playrate+=direction_playrate;
    if (::y_playrate<0) ::y_playrate=0;
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
   * set_normalperiod_metarate on the other hand will update the index file; which 
   * might not be wanted.
   */
  virtual void set_normalperiod_metarate(quad_period_type newnormalperiod_metarate)
  {
    if (currentperiod_metarate==normalperiod_metarate)
      currentperiod_metarate=normalperiod_metarate=newnormalperiod_metarate;
    else
      {
	normalperiod_metarate=newnormalperiod_metarate;
	if (normalperiod_metarate>0)
	  ::y_playrate = ::x_diskrate * currentperiod_metarate * dsp->playrate 
	      / ( normalperiod_metarate * diskrate);
      }
  }
};

extern clock_driver *metronome;

#endif // __loaded__clock_drivers_h__
