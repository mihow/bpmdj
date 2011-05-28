/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__dsp_jack_h__
#define __loaded__dsp_jack_h__
using namespace std;
#line 1 "dsp-jack.h++"
#ifdef COMPILE_JACK
#include "version.h"
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <pthread.h>
#include "dsp-drivers.h"

typedef jack_default_audio_sample_t sample_t;

class dsp_jack: public dsp_driver
{
private:
  bool verbose;
  char *     arg_dev;      // the device string from the configuration file
  char * lout;
  char * rout;
  jack_port_t *output_port_1;
  jack_port_t *output_port_2;
  jack_client_t *client;
  int fill_empty_buffer(jack_nframes_t);
  int generate_buffer(jack_nframes_t);
  int bufsize;
  size_t minfill;
  jack_ringbuffer_t *buf1;
  jack_ringbuffer_t *buf2;
public:
  dsp_jack(const PlayerConfig & config);
  void    start(audio_source* s);
  void    stop();
  void    internal_pause() {}
  void    internal_unpause() {}
  void    write(stereo_sample2 value) {}
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
  int set_buffer_size (jack_nframes_t nframes);
  void process_buffers();
  jack_client_t *get_client() { return client; }
  jack_port_t *get_output_port_1() { return output_port_1; }
  jack_port_t *get_output_port_2() { return output_port_2; }
};


bool is_jack_driver();
#endif
#endif // __loaded__dsp_jack_h__
