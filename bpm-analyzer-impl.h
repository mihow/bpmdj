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
#ifndef __loaded__bpm_analyzer_impl_h__
#define __loaded__bpm_analyzer_impl_h__
using namespace std;
#include <boost/smart_ptr.hpp>
#include "analyzer-impl.h"

class BpmAnalyzerImpl: public AnalyzerImpl
{
protected:
  const bool show_as_tempo;
  const int samplerate;
  Axis* hor;
  // scan bounds
  unsigned4 startbpm, stopbpm;
  // the audio as was passed along when constructing this analyzer.
  stereo_sample2* input_audio;
  // how many samples are contained in the input_audio
  signed4 input_samples;
public:
  float8 tempo;
  BpmAnalyzerImpl(unsigned4 sr, bool sat=true): 
    show_as_tempo(sat), samplerate(sr), hor(NULL)
  {
  }
  Axis* horizontal()
  {
    if (hor) return hor;
    if (show_as_tempo) 
      hor=axis("Tempo (qMPM)");
    else 
      hor=axis("Measure length (secs)");
    hor->show_grid=true;
    return hor;
  }
  void period_point(int z, float8 per, float8 y)
  {
    if (show_as_tempo)
      {
	double bpm=4*60*samplerate/per;
	if (bpm>=(double)startbpm && bpm<=(double)stopbpm)
	  point(z,bpm,y);
      }
    else
      point(z,per/samplerate,y);
  }
};

typedef boost::shared_ptr<BpmAnalyzerImpl> BpmAnalyzer;
#endif // __loaded__bpm_analyzer_impl_h__
