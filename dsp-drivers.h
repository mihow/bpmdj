/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
class dsp_driver
{
 protected:
  bool verbose;
 public:
  // playing
  dsp_driver(const PlayerConfig & config) { verbose = false; };
  virtual void start() = 0;
  virtual void pause() = 0;
  virtual void write(stereo_sample2 value) = 0;
  virtual signed8 latency() = 0;
  virtual int  open() = 0;
  virtual void close(bool flush_first) = 0;
  virtual bool is_none() {return false; };
  virtual ~dsp_driver() {};
  static dsp_driver * get_driver(PlayerConfig * cfg);
};

typedef dsp_driver * dsp_driver_ptr;
#endif // __loaded__dsp_drivers_h__
