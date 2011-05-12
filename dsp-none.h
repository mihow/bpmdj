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
#ifndef __loaded__dsp_none_h__
#define __loaded__dsp_none_h__
using namespace std;
#line 1 "dsp-none.h++"
#include "version.h"
#include "player-config.h"
#include "dsp-drivers.h"
#include "common.h"

class dsp_none: public dsp_driver
{
public:
  dsp_none(const PlayerConfig & config);
  void    start(audio_source*);
  void    stop();
  void    internal_pause() {};
  void    internal_unpause() {};
  void    write(stereo_sample2 value);
  signed8 latency();
  int     open(bool ui);
  void    close(bool);
};

bool is_none_driver();
#endif // __loaded__dsp_none_h__
