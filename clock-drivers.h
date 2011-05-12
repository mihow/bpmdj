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
#ifndef __loaded__clock_drivers_h__
#define __loaded__clock_drivers_h__
using namespace std;
#line 1 "clock-drivers.h++"
#include "types.h"
#include "period-type.h"

/**
 * This variable denotes the targetperiod, which is the length of 1 measure (=4 beats)
 * at the command line requested playing speed
 */
extern quad_period_type targetperiod;

/**
 * This variable denotes the currentperiod, which is the length of 1 measure (=4 beats)
 * which is used to play the song. The tempo must be set through
 * the changetempo() function.
 */
extern quad_period_type currentperiod;

/**
 * This variable denotes the currentperiod, which is the length of 1 measure (=4 beats)
 * at the tempo which is native to the current song.
 */
extern quad_period_type normalperiod;

/**
 * will modify the tempo and informt he current metronome
 */
void changetempo(signed8);

/**
 * x is the data position in the raw file (thus at normal tempo)
 */
extern signed8 x;

/**
 * y is the position in the playing file (thus at target tempo)
 */
extern signed8 y;

/**
 * converts the y position (samples given the current tempo) to x (playing position in the raw file)
 */
signed8 x_normalise(signed8 y);

/**
 * covnerts the x position (samples in the raw file) to a position given the current tempo.
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
 * - make the clock follow certain marking points as good as possible (with or without using the tempo ?)
 * - make the clock emit synchronisation points based on the last cue.
 */
class clock_driver
{
protected:
  bool verbose;
public:
  // playing
  clock_driver()
  { 
    verbose = true;
  };
  /**
   * Initialize the driver by allocating necessary
   * threads and data structures. The current dsp driver
   * is passed as argument into this function.
   */
  virtual void init()=0;
  /**
   * The clock driver is already running but the user
   * started from a specific cue again
   */
  virtual void cue_start()=0;
  /**
   * Called when the tempo is about tho change.
   */
  virtual void tempo_will_change(signed8 to)=0;
};

extern clock_driver *metronome;
#endif // __loaded__clock_drivers_h__
