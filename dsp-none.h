/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__dsp_none_h__
#define __loaded__dsp_none_h__
using namespace std;
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
  void    unpause() {};
  void    write(stereo_sample2 value);
  signed8 latency();
  int     open(bool ui);
  void    close(bool);
};

bool is_none_driver();
bool is_none_driver(dsp_driver* what);
#endif // __loaded__dsp_none_h__
