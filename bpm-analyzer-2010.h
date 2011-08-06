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
#ifndef __loaded__bpm_analyzer_2010_h__
#define __loaded__bpm_analyzer_2010_h__
using namespace std;
#include <fftw3.h>
#include <cstdio>
#include <qstring.h>
#include "stereo-sample2.h"
#include "common.h"
#include "bpm-analyzer-impl.h"
#include "tempo-type.h"

typedef float8 fft_type;

class BpmAnalyzer2010: public BpmAnalyzerImpl
{
private:
  void map_output(float4* target, int startfreq, int count, double* hcfreq, int winsize);
  void spectrum_to_bark(fftw_complex* in, int window_size, double* out);
public:
  BpmAnalyzer2010(stereo_sample2* audio, int n, int sample_rate, float from_tempo, float to_tempo);
  virtual bool step();
};
#endif // __loaded__bpm_analyzer_2010_h__
