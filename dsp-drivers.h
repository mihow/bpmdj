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
#ifndef __loaded__dsp_drivers_h__
#define __loaded__dsp_drivers_h__
using namespace std;
#line 1 "dsp-drivers.h++"
#include "common.h"
#include "stereo-sample2.h"
#include "player-config.h"
#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#ifndef COMPILE_JACK
#error -------------------------------------------
#error Should at least compile one dsp driver !!!
#error Use Try -D COMPILE_OSS, -D COMPILE_ALSA or
#error -D COMPILE_JACK
#error -------------------------------------------
#endif
#endif
#endif

/*-------------------------------------------
 *         Forward
 *-------------------------------------------*/
class PlayerConfig;

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/

/**
 * BpmDj can use any sound device driver necessary given that a class exists to
 * support such a driver. For instance, we scurrently have an Alsa driver, an old 
 * fashioned Oss driver and an experimental Jack driver. Examnples of drivers
 * can be found in dsp-alsa.h and dsp-oss.h.
 */

class dsp_driver
{
 protected:
  bool verbose;
 public:
  // playing
  dsp_driver(const PlayerConfig & config) { verbose = false; };
  virtual void start() = 0;
  /**
   * The pause function is called from within the player
   * and should stop the playback. Then it should call the
   * wait_for_unpause routine and when that one returns
   * it should continue playing. This is arranged such that deadlocks
   * will not occur and that double calls will not occur either.
   */
  virtual void pause() = 0;
  /**
   * The write function should write out the specified
   * value and block when necessary.
   */
  virtual void write(stereo_sample2 value) = 0;
  virtual signed8 latency() = 0;
  /**
   * The open function should intialize the specific device
   * such that we can start writing to it and that it will
   * play sound. If the open is not possible this function
   * should return err_dsp. If the function is successfull
   * open() should return err_none
   */
  virtual int  open() = 0;
  virtual void close(bool flush_first) = 0;
  virtual bool is_none() {return false; };
  virtual ~dsp_driver() {};
  static dsp_driver * get_driver(PlayerConfig * cfg);
};

typedef dsp_driver * dsp_driver_ptr;
#endif // __loaded__dsp_drivers_h__
