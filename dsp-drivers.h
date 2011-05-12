/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle

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

#ifndef DSP_DRIVERS
#define DSP_DRIVERS
#include "common.h"
#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#error -------------------------------------------
#error Should at least compile one dsp driver !!! 
#error Try using -D COMPILE_OSS, -D COMPILE_ALSA 
#error -------------------------------------------
#endif
#endif

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/
class dsp_driver
{
 protected:
  bool verbose;
 public:
  // playing
  dsp_driver() { verbose = false; };
  virtual void start() = 0;
  virtual void pause() = 0;
  virtual void write(unsigned4 *value) = 0;
  virtual signed8 latency() = 0;
  virtual int  open() = 0;
  virtual void close() = 0;
  // options
  virtual int parse_option(char* option, char* argument);
};

#endif
