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
#ifndef __loaded__dsp_jack_cpp__
#define __loaded__dsp_jack_cpp__
using namespace std;
#line 1 "dsp-jack.c++"
#ifdef INCOMPLETE_FEATURES
#ifdef COMPILE_JACK
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <qdialog.h>
#include <qstring.h>
#include "player-core.h"
#include "version.h"
#include "dsp-jack.h"
#include "memory.h"

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
void dsp_jack::start()
{
   printf("dsp_jack::start called\n");
}

void dsp_jack::pause()
{
  // stop the device output
   printf("dsp_jack::pause -- pause\n");
  wait_for_unpause();
  // simply continue
   printf("dsp_jack::pause -- continue\n");
}

void dsp_jack::write(stereo_sample2 value)
{
}

signed8 dsp_jack::latency()
{
   printf("dsp_jack::latency\n");
   return 0;
}

dsp_jack::dsp_jack(const PlayerConfig & config) : dsp_driver(config)
{
   arg_dev = strdup(config.get_jack_dev());
   arg_latency = config.get_jack_latency();
   verbose = config.get_jack_verbose();
   printf("dsp_jack::<constructor> called\n");
   printf("  device = %s\n",arg_dev);
   printf("  latency = %d\n",arg_latency);
   printf("  verbose = %d\n",verbose);
}

int dsp_jack::open()
{
   printf("dsp_jack::open called\n");
   /**
    * This routine must set the playing speed to WAVRATE
    * 2 channels and 16 bit.
    */
   
   /*
    if (err < 0)
    {
      printf("dsp: opening jack device failed : %s\n",snd_strerror(err));
      return err_dsp;
    }
    */
  
  start();
  return err_none;
}

void dsp_jack::close()
{
   printf("dsp_jack::close called\n");
}
#endif
#endif
#endif // __loaded__dsp_jack_cpp__
