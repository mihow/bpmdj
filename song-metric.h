/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****/

#ifndef SONG_METRIC_H
#define SONG_METRIC_H
#include <qstring.h>
#include <time.h>
#include <qcolor.h>
#include <qpixmap.h>
#include "qvectorview.h"
#include "data.h"
#include "config.h"
#include "cluster.h"
#include "index.h"
#include "metric-widget.h"

class Song;
QString tonumber(const int b);

#define no_distance 1000
class SongMetriek: public Metriek
{
 public: 
  
  // weights
  float tempo;
  float spectrum;
  float histogram;
  float rythm;
  float composition;

  // ranges
  accessors(float,tempo_limits,0.05); // tempo_limits input from configuration

 private:
  float tempo_scale;     // thge tempo scale in the log range
  float total;           // the total weight
  bool  harmonic545;     // which harmonics do we take into account
  bool  harmonic646;     
  bool  harmonic747;
  bool  harmonic848;
  float harmonic54_term; 
  float harmonic64_term;
  float harmonic74_term;
  float harmonic84_term;
  float harmonic45_term;
  float harmonic46_term;
  float harmonic47_term;
  float harmonic48_term;
  bool  unknown_tempo_matches;

 public:
  
  SongMetriek(float tw, float sw, float hw, float rw, float cw);
  SongMetriek(MetricWidget & take_from);
  void  set_show_harmonics(bool h5, bool h6,bool h7,bool h8);
  void  set_unknown_tempo_matches(bool v) {unknown_tempo_matches = v;};
  void  set_tempo_weight(float val) {tempo = val;};
  void  set_spectrum_weight(float val) {spectrum = val;};
  void  set_echo_weight(float val) { histogram = val;};
  void  set_rythm_weight(float val) { rythm = val; };
  void  set_composition_weight(float val) { composition = val; };
  
  void  prepare(); 
  float tempo_diff(const Song& a, const Song& b) const;
  float tempo_dist(const Song& a, const Song& b) const {return fabs(tempo_diff(a,b));};
  float spectrum_dist(const Song &, const Song & song) const;
  float histogram_dist(const Song &, const Song & song, double breakat) const;
  float rythm_dist(const Song &, const Song & song, double breakat) const;
  float composition_dist(const Song & a, const Song & song, double breakat) const;
  float find_harmonic(float td, int & which) const;
  float distance(const Song &a, const Song &b, double limit) const;

 public:
  static SongMetriek std;
};

#endif
