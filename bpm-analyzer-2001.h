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
#ifndef __loaded__bpm_analyzer_2001_h__
#define __loaded__bpm_analyzer_2001_h__
using namespace std;
#include "stereo-sample2.h"
#include "common.h"
#include "bpm-analyzer-impl.h"

/**
 * The ordinary rayshoot scanner as published at
 * http://werner.yellowcouch.org/Papers/bpm04/
 */
class BpmAnalyzer2001:  public BpmAnalyzerImpl
{
  unsigned1 * audio;
  signed4   audiosize;
  unsigned4 stopshift, startshift;
  signed4   bufsiz;
  // finding the error fit of a curve
  unsigned4 phasefit(unsigned4 i);
  unsigned4 phasefit(unsigned4 i, unsigned4 clip);
  void rayshoot_scan();
  // Reads the file in memory divided by blocks
  void readAudioBlock(signed4 blocksize);  
public:
  BpmAnalyzer2001(stereo_sample2 *block, unsigned4 sample_length, 
		  float8 lower_boundary, float8 higher_boundary);
  virtual bool step();
  virtual ~BpmAnalyzer2001();
};
#endif // __loaded__bpm_analyzer_2001_h__
