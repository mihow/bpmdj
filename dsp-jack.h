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
#ifndef __loaded__dsp_jack_h__
#define __loaded__dsp_jack_h__
using namespace std;
#line 1 "dsp-jack.h++"
#ifdef COMPILE_JACK
#include "version.h"
#include <jack/jack.h>
#include <pthread.h>
#include "dsp-drivers.h"

class dsp_jack: public dsp_driver
{
  private:
   bool      verbose;
   char *     arg_dev;      // the device string from the config file
   jack_port_t *output_port_1;
   jack_port_t *output_port_2;
   jack_client_t *client;
   unsigned4 buffer_size;
   unsigned4 * buffer;
  /**
   * The position to which the buffer is filled
   * The enxt sample will be written into address
   * buffer+filled.
   */
   unsigned4 filled;
  /**
   * The current playing position in the buffer
   * When the jack threat calls us back we start to deliver data
   * from buffer+position on.
   */
  unsigned4 position; 
 public:
   dsp_jack(const PlayerConfig & config);
   void    start();
   void    pause();
   void    write(stereo_sample2 value);
   signed8 latency();
   int     open();
   void    close(bool flush_first);
   /**
    * This is the function that will be called back
    * from within the jack audio thread. In this function
    * we should provide the next chunk of data.
    */
   int audio_process (jack_nframes_t nframes);
};

#endif
#endif // __loaded__dsp_jack_h__
