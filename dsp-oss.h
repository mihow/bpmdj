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
#ifndef __loaded__dsp_oss_h__
#define __loaded__dsp_oss_h__
using namespace std;
#line 1 "dsp-oss.h++"
#ifdef COMPILE_OSS
#include <linux/soundcard.h>
#include "dsp-drivers.h"

class dsp_oss: public dsp_driver
{
 private:
  int            dsp;
  signed8        dsp_writecount;
  audio_buf_info dsp_latency;
  char * arg_dsp;
  int    arg_fragments;
  int    opt_fragments;
  int    opt_nolatencyaccounting;
  int    arg_latency ;
  signed8 playcount();
  void flush();
 public:
  dsp_oss(const PlayerConfig & config);
  void    start(audio_source*);
  void    internal_pause();
  void    internal_unpause();
  void    write(stereo_sample2 value);
  signed8 latency();
  int     open(bool ui);
  void    close(bool flush);
};
#endif
#endif // __loaded__dsp_oss_h__
