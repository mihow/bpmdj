/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
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
  bool synchronous;
  bool verbose;
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
  unsigned4 jack_time;
  int fill_empty_buffer(jack_nframes_t);
  int get_buffer(jack_nframes_t);
  int generate_buffer(jack_nframes_t);
public:
  dsp_jack(const PlayerConfig & config);
  void    start(audio_source* s);
  void    stop();
  void    internal_pause();
  void    internal_unpause();
  void    write(stereo_sample2 value);
  signed8 latency();
  int     open(bool ui);
  void    close(bool flush_first);
  virtual ~dsp_jack();
  /**
   * This is the function that will be called back
   * from within the jack audio thread. In this function
   * we should provide the next chunk of data. Depending
   * on whether the driver is in synchronous or asynchronous
   * mode do we either call get_next_buffer or 
   * generate_next_buffer
   */
  int audio_process (jack_nframes_t nframes);
  jack_client_t *get_client() { return client; }
  jack_port_t *get_output_port_1() { return output_port_1; }
  jack_port_t *get_output_port_2() { return output_port_2; }
  unsigned4 get_jack_time() { return jack_time; }
  void set_jack_time(unsigned4 t) { jack_time = t; }
};

/**
 * used for the async jack driver
 */
int jack_process(jack_nframes_t nframes, void *ignored);
bool is_jack_driver();
#endif
#endif // __loaded__dsp_jack_h__
