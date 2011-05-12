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
#ifndef __loaded__clock_drivers_h__
#define __loaded__clock_drivers_h__
using namespace std;
#line 1 "clock-drivers.h++"
#include "types.h"
#include "period-type.h"

/**
 * This variable denotes the targetperiod, which is the length of 1 measure 
 * (=4 beats) at the command line requested playing speed
 */
extern quad_period_type targetperiod;

/**
 * This variable denotes the currentperiod, which is the length of 1 measure 
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
 * will modify the tempo and informt he current metronome
 */
void changetempo(signed8);

/**
 * x is the data position in the raw file (thus at normal tempo)
 * This is calcualted during streaming. So updating this variable 
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
 * In other words. this is a tricky variable which is afterwards transformed
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

/**
 * The clock drivers make it possible to synchronize with external clocks
 * or to emit an external clock. At the moment there is only
 * one clock capable driver which is the Jack transport code.
 * Currently we are still exploring what interface would be appropriate
 * here. I can imagine a couple of uses
 * - make the clock driver alter the tempo when requested instead of the player
 * - make the clock driver set the tempo in sync with external actors
 * - make the clock emit a tempo setting
 * - make the clock follow certain marking points as good as possible (with or 
 *   without using the tempo ?)
 * - make the clock emit synchronisation points based on the last cue.
 *
 * These are a collection of functions that 
 * might be necessary for a clockdriver.
 * A clockdriver 
 * - can emit the current tempo
 * - can emit the current playing position
 * - can receive the required (target) tempo
 * - can receive an updated (native) tempo
 * - can receive the current playing position
 * 
 * A clock driver should also have the possibility to switch its mode of
 * operation independently or to receive a new manual set mode of operation.
 * - can run in Auto, Master or Slave mode
 * 
 * Whether the clockdriver should also perform the source to target slide 
 * is not clear. 
 *
 * How do we deal with a tempo alteration set in the user interface ?
 * E.g: the wantedtargetperiod
 *
 * Clockdrivers need to know what the total playing time is so far. Whether this
 * should be directly dependent on the sound driver or not is not yet clear.
 *
 * More docu to think about:
 * - at the moment, timemaster tells if the player emit tempo / position
 *   information (true) or receives it (false). Additionally, there should
 *   be an "internal" mode in which no external clock is used (current
 *   behavior without JACK_TRANSPORT).
 * - the position within the song is still kept by the "y" variable
 * - in addition, jack_time counts time since the transport was started,
 *   shifted a bit so that time 0 is always the start of a bar (needed for
 *   sync with other applications).
 * 
 * In slave mode, sync_with_jack gets tempo from jack and updates
 * currentperiod if needed. When a request for sync (beatmatching) is
 * done, it also shifts y to the same position relative to the current
 * bar as jack_time reports.
 * 
 * Now jack_time could be transformed to clock_time, synced with any
 * clock source we want (internal, jack, ...)
 */ 

class clock_driver
{
protected:
  signed4 offset;
  bool verbose;
public:
  // playing
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
   * The clock driver is already running but the user
   * started from a specific cue again
   */
  virtual void cue_start()
  {
  };
  virtual void request_sync(int retries)
  {
  };

  virtual void shift(signed4 direction)
  {
    ::y+=direction;
    if (::y<0) ::y=0;
  }
  /**
   * Called when the tempo is about tho change.
   */
  virtual void tempo_will_change(signed8 to)
  {
  };

  /**
   * This function is called by the routines that can detect or modify the
   * native tempo. When overriding this function, you should call the superclass
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
};

extern clock_driver *metronome;

extern unsigned8 beat, bar, tick, clockframes;
extern bool timemaster;

void set_normalperiod(quad_period_type newnormalperiod, 
		      bool update_on_disk=true);
#endif // __loaded__clock_drivers_h__
