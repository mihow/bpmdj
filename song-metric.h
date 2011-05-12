/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__song_metric_h__
#define __loaded__song_metric_h__
using namespace std;
#line 1 "song-metric.h++"
#include <qstring.h>
#include <time.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "data.h"
#include "config.h"
#include "cluster.h"
#include "index.h"
#include "metric-widget.h"

class Song;

#define no_distance 1000
class SongMetriek: public Metriek
{
public: 
  // weights
  float4 tempo;
  float4 spectrum;
  float4 histogram;
  float4 rhythm;
  float4 composition;

  // ranges
  accessors(float4,tempo_limits,0.05); // tempo_limits input from configuration

private:
  float4 tempo_scale;     // the tempo scale in the log range
  float4 total;           // the total weight
  bool  harmonic545;     // which harmonics do we take into account
  bool  harmonic646;     
  bool  harmonic747;
  bool  harmonic848;
  float4 harmonic54_term; 
  float4 harmonic64_term;
  float4 harmonic74_term;
  float4 harmonic84_term;
  float4 harmonic45_term;
  float4 harmonic46_term;
  float4 harmonic47_term;
  float4 harmonic48_term;
  bool  unknown_tempo_matches;

public:
  SongMetriek(float4 tw, float4 sw, float4 hw, float4 rw, float4 cw);
  SongMetriek(MetricWidget & take_from);
  void  set_show_harmonics(bool h5, bool h6,bool h7,bool h8);
  void  set_unknown_tempo_matches(bool v) {unknown_tempo_matches = v;};
  void  set_tempo_weight(float4 val) {tempo = val;};
  void  set_spectrum_weight(float4 val) {spectrum = val;};
  void  set_echo_weight(float4 val) { histogram = val;};
  void  set_rhythm_weight(float4 val) { rhythm = val; };
  void  set_composition_weight(float4 val) { composition = val; };
  
  void  prepare(); 
  float4 tempo_diff(const Song& a, const Song& b) const;
  float4 tempo_dist(const Song& a, const Song& b) const 
  {
    return fabs(tempo_diff(a,b));
  };
  float4 spectrum_dist(const Song &, const Song & song) const;
  float4 histogram_dist(const Song &, const Song & song, float8 breakat) const;
  float4 rhythm_dist(const Song &, const Song & song, float8 breakat) const;
  float4 composition_dist(const Song & a, const Song & song, 
			  float8 breakat) const;
  float4 find_harmonic(float4 td, int & which) const;
  float4 distance(const Song &a, const Song &b, float8 limit) const;

 public:
  static SongMetriek std;
};
#endif // __loaded__song_metric_h__
