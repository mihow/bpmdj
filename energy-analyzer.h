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
#ifndef __loaded__energy_analyzer_h__
#define __loaded__energy_analyzer_h__
using namespace std;
#line 1 "energy-analyzer.h++"
#include "analyzer.h"
#include "common.h"
#include "stereo-sample2.h"

float4 normalization_factor(signed2 mi, signed2 ma, signed2 me);

class EnergyAnalyzer: public Analyzer
{
 private:
  unsigned8 audiosize;
  stereo_sample2 *audio;
 public:
  EnergyAnalyzer() {};
  void readAudio();
  virtual void analyze();
};
#endif // __loaded__energy_analyzer_h__
