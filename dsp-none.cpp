/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#include "scripts.h"

dsp_none::dsp_none(const PlayerConfig & config) : dsp_driver(config) 
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_none::dsp_none<constructor>");
#endif
  paused=true;
};

/**
 * This function overrides the standard behavior such 
 * that we do not start an unnecessary thread
 */
void dsp_none::start(audio_source* a)
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_none::start()");
#endif
}

/**
 * This function overrides the standard behavior
 * such that it does not stop a thread that was not started
 */
void dsp_none::stop()
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_none::stop()");
#endif
}

void dsp_none::write(stereo_sample2 value)
{
  /**
   * This should not happen.
   */
  assert(0);
}

signed8 dsp_none::latency()
{
  return 0;
}

int dsp_none::open(bool ui)
{
  return err_none;
}

void dsp_none::close(bool flushfirst)
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_none::close()");
#endif
}

bool is_none_driver()
{
  return typeid(*dsp) == typeid(dsp_none);
}

bool is_none_driver(dsp_driver* dsp)
{
  return typeid(*dsp) == typeid(dsp_none);
}
#endif // __loaded__dsp_none_cpp__
