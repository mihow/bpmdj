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
#ifndef __loaded__clock_jack_cpp__
#define __loaded__clock_jack_cpp__
using namespace std;
#line 1 "clock-jack.c++"
#ifdef COMPILE_JACK
#ifdef JACK_TRANSPORT
#include <typeinfo>
#include "clock-jack.h"
#include "dsp-jack.h"
#include "player-core.h"
#include "scripts.h"

signed8 first_beat=0;
bool autosync = false;
unsigned4 last_seen;

clock_jack::clock_jack(const PlayerConfig& config)
{
  // WVB -- whatever you need here
  autosync = config.get_jack_receiveposition();
  offset=0;
}

void clock_jack::init()
{
  printf("clock_jack::init()\n");
  if (is_jack_driver())
    {
      if (currentperiod > 0) 
	{
	  update_first_beat();
	  request_sync(20);
	  /* create jack transport thread */
	  pthread_t *jt = bpmdj_allocate(1,pthread_t);
	  pthread_create(jt,NULL,sync_with_jack,NULL);
	}
    }
  else
    {
      /**
       * TODO: We don't have a possibility here to return
       * an error value and reset the clock driver
       */
      Fatal("The Jack Clock transport cannot be started without "
	    "a jack DSP driver\n");
    }
  offset=0;
}

void clock_jack::shift(signed4 direction)
{
  clock_driver::shift(direction);
  offset+=direction;
}

void clock_jack::cue_start()
{
  if (currentperiod > 0) 
    {
      update_first_beat();
      request_sync(3);
    }
}

void clock_jack::tempo_will_change(signed8 t)
{
  if (!timemaster)
    become_master(0);
}

void update_first_beat() 
{
  first_beat = cue;
  if (normalperiod > 0) 
    while (first_beat > normalperiod) 
      first_beat -= normalperiod;
}

void timebase(jack_transport_state_t state, jack_nframes_t nframes, 
	      jack_position_t *pos, int new_pos, void *arg) 
{
  jack_client_t *client = ((dsp_jack *)dsp)->get_client();
  float8 bpm;
    
  bpm = mperiod2bpm(currentperiod);
  pos->beats_per_minute = bpm;
  pos->valid = JackPositionBBT;
  pos->ticks_per_beat = 1920.;
  pos->beats_per_bar = pos->beat_type = 4.; /* we assume 4/4 bars... */
  if (x > first_beat) {
    pos->frame_rate = 44100;
    /* 
       Tricky part: we want jack time to be global time,
       but it should also be relative to a measure start, which is
       impossible with tempo changes.
       So we drift time a little bit if needed...
    */
    
    unsigned4 jack_time = ((dsp_jack *)dsp)->get_jack_time();
    signed8 z1 = (y-y_normalise(first_beat)) % currentperiod;
    signed8 z2 = jack_time % currentperiod;
    jack_time += (z1-z2);
    
    pos->frame = jack_time;
    ((dsp_jack *)dsp)->set_jack_time(jack_time);
    pos->bar = (x-first_beat)/normalperiod;
    pos->beat = (x- first_beat - pos->bar*normalperiod)/(normalperiod/4); 
    pos->tick = (x- first_beat - pos->bar*normalperiod - pos->beat*
		 (normalperiod/4)) /(normalperiod/7680);
    pos->bar ++;
    pos->beat ++;
  }
  
  jack_transport_reposition(client,pos);
  last_seen = pos->frame;
}

void * sync_with_jack(void* neglect) 
{
  dsp_jack *dj = (dsp_jack *)dsp;
  jack_client_t *client = dj->get_client();
  jack_position_t pos;
  signed8 z1,z2;
  
  while (1) 
    {
      if (dsp->get_paused() || dsp->get_stopped()) 
	{
	  timemaster = false;
	  jack_release_timebase(((dsp_jack *)dsp)->get_client());
	  if (dsp->get_stopped())
	    return neglect;
	}
      
      jack_transport_state_t state = jack_transport_query(client, &pos);

      beat = pos.beat;
      bar = pos.bar;
      tick = pos.tick;
      clockframes = pos.frame;

      if (timemaster) 
	{
	  if (state == JackTransportRolling) 
	    {
	      if (pos.frame > last_seen + 16384) 
		{
		  timemaster = false;
		}
	      
	    }
	}
      if (!timemaster) 
	{
	  if (!dsp->get_paused()) 
	    {
	      if (!(pos.valid & JackPositionBBT)) 
		{
		  become_master(1);
		}
	      if (state == JackTransportRolling) 
		{
		  /* getting BPM from jack */
		  /* remark: mperiod2bpm also does the reverse, bpm2period */
		  if (pos.valid & JackPositionBBT) 
		    {
		      int jbpm = (int) mperiod2bpm(pos.beats_per_minute);
		      if (currentperiod != jbpm) 
			{
			  targetperiod = currentperiod = jbpm;
			  y = x*currentperiod/normalperiod;
			}
		    }
		  
		  if (autosync) 
		    {
		      /**
		       * Syncing position to the same position within bar as
		       * jack 
		       */
		      z1 = (y-metronome->get_offset()-y_normalise(first_beat))
			% currentperiod;
		      z2 = pos.frame % currentperiod;
		      if (z2-z1 > currentperiod / 2) 
			{
			  z1 += currentperiod;
			}
		      else if (z1-z2 > currentperiod / 2) 
			{
			  z2 += currentperiod;
			}		     
		      if (abs(z2-z1) > 2048)
			y += (z2-z1);
		      while (y < 0)
			y += currentperiod;
		    }
		}
	    }
	}
      usleep(250);
    }
  
  return neglect;
}

void clock_jack::request_sync(int retries) 
{
  //needs_sync += retries;
}

void become_master(int cond) 
{
  dsp_jack *dj = (dsp_jack *)dsp;
  jack_client_t *client = dj->get_client();
  jack_position_t pos;
  
  if (!timemaster) 
    {
      if (jack_set_timebase_callback(client, cond, timebase, NULL) == 0) 
	{
	  timemaster = true;
	  if (jack_transport_query(client, &pos) == JackTransportStopped) 
	    {
	      jack_transport_start(client);
	      dj->set_jack_time(0);
	      last_seen = 0;
	    }
	  else {
	    dj->set_jack_time(pos.frame);
	    last_seen = pos.frame;
	  }
	}
    }
}

#endif
#endif
#endif // __loaded__clock_jack_cpp__
