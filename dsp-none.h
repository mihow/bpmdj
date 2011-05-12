/****
 BpmDj: Free Dj Tools
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
#ifndef __BPMDJ___DSP_NONE_H__
#define __BPMDJ___DSP_NONE_H__
using namespace std;
#line 1 "dsp-none.h++"
#include "version.h"
#include "player-config.h"
#include "dsp-drivers.h"
#include "common.h"

class dsp_none: public dsp_driver
{
    signed4 samples_to_wait;
  public:
    dsp_none(const PlayerConfig & config) : dsp_driver(config) {};
    void    start();
    void    pause();
    void    write(stereo_sample2 value);
    signed8 latency();
    int     open();
    void    close(bool);
    virtual bool is_none() { return true;};
};
#endif
