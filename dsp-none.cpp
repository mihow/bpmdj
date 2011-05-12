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
#ifndef __loaded__dsp_none_cpp__
#define __loaded__dsp_none_cpp__
using namespace std;
#line 1 "dsp-none.c++"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <qdialog.h>
#include <qstring.h>
#include <string.h>
#include <unistd.h>
#include "player-core.h"
#include "version.h"
#include "dsp-none.h"

void dsp_none::start()
{
  samples_to_wait=0;
}

void dsp_none::pause()
{
  wait_for_unpause();
}

void dsp_none::write(stereo_sample2 value)
{
  /**
   * The usleep routine does not always work properly
   * Since this is a less useful driver, we simply
   * gather dat for up to a second and then wait
   * one second.
   * usleep(1000000/44100);
   */
  samples_to_wait++;
  if (samples_to_wait>44100)
    {
      samples_to_wait-=44100;
      sleep(1);
    }
}

signed8 dsp_none::latency()
{
  return samples_to_wait;
}

int dsp_none::open()
{
  return err_none;
}

void dsp_none::close(bool flushfirst)
{
  if (flushfirst)
    while (samples_to_wait>44100)
      {
	samples_to_wait-=44100;
	sleep(1);
      }
}
#endif // __loaded__dsp_none_cpp__
