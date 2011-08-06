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
#ifndef __loaded__bpm_analyzer_wec_h__
#define __loaded__bpm_analyzer_wec_h__
using namespace std;
#include <fftw3.h>
#include "type-operations.h"
#include "bpm-analyzer-impl.h"

/**
 * Bpm Analyzer - Weihgted Envelope Autocorrelation
 * ------------------------------------------------
 * A tempo measurement algorithm based on calculatin the autocorrelation of the 
 * energy envelope and weighing through a energy envelope spectrum.
 */
class BpmAnalyzerWec: public BpmAnalyzerImpl
{
private:
  unsigned4 measured;
  float8* audio;
  unsigned4 b1, b2;
  float8 * en;
  float8 * ts;
  float8 * co;
  fftw_plan forward;
  fftw_plan backward;
  const int measure_rate;
public:
  BpmAnalyzerWec(stereo_sample2 *block, unsigned4 sample_length, 
		 int wavrate, float8 lower_boundary, float8 higher_boundary):
    BpmAnalyzerImpl(wavrate,true),
    measure_rate(11025)
  {
    input_audio=block;
    input_samples=sample_length;
    startbpm=lower_boundary;
    stopbpm=higher_boundary;
    init();
  };
  ~BpmAnalyzerWec();
  void init();
  virtual bool step();
};
#endif // __loaded__bpm_analyzer_wec_h__
