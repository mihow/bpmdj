/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__song_metric_cpp__
#define __loaded__song_metric_cpp__
using namespace std;
#line 1 "song-metric.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <qspinbox.h>
#include <sys/stat.h>
#include <qpainter.h>
#include "qsong.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum-type.h"
#include "composition-property.h"
#include "tags.h"
#include "memory.h"
#include "statistics.h"
#include "song-metric.h"
#include "metric-widget.h"

SongMetriek::SongMetriek(float4 tw, float4 sw, float4 hw, float4 rw, float4 cw)
{
  tempo = tw;
  spectrum = sw;
  histogram = hw;
  rhythm = rw;
  composition = cw;
  harmonic545 = harmonic646 = harmonic747 = harmonic848 = false;
  unknown_tempo_matches = false;
};

SongMetriek::SongMetriek(MetricWidget & take_from)
{
  set_tempo_limits(std.get_tempo_limits());
  set_show_harmonics(std.harmonic54_term, std.harmonic64_term, 
		     std.harmonic74_term, std.harmonic84_term);
  unknown_tempo_matches = false;
  
  set_tempo_weight(    (float4)(take_from.tempoDistanceSpin->value())/100.0 );
  set_spectrum_weight( (float4)(take_from.spectrumSpin->value())/100.0);
  set_echo_weight(     (float4)(take_from.echoSpin->value())/100.0);
  set_rhythm_weight(    (float4)(take_from.rhythmSpin->value())/100.0);
  std.set_composition_weight((float4)(take_from.compositionSpin->value())/
			     100.0);
  
  prepare();
}

float4 SongMetriek::find_harmonic(float4 td, int & which) const
{
  float4 d;
  which = 0;
  if (fabs(td) <= 1.0) return td;
  which = 1;
  if (harmonic545 && fabs(d = td + harmonic54_term)<=1.0) return d;
  if (harmonic545 && fabs(d = td + harmonic45_term)<=1.0) return d;
  which = 2;
  if (harmonic646 && fabs(d = td + harmonic64_term)<=1.0) return d;
  if (harmonic646 && fabs(d = td + harmonic46_term)<=1.0) return d;
  which = 3;
  if (harmonic747 && fabs(d = td + harmonic74_term)<=1.0) return d;
  if (harmonic747 && fabs(d = td + harmonic47_term)<=1.0) return d;
  which = 4;
  if (harmonic848 && fabs(d = td + harmonic84_term)<=1.0) return d;
  if (harmonic848 && fabs(d = td + harmonic48_term)<=1.0) return d;
  which = 0;
  return no_distance;
}

float4 SongMetriek::tempo_diff(const Song& a, const Song& b) const
{
  tempo_type fa = a.get_tempo();
  tempo_type fb = b.get_tempo();
  if (fa.none() || fb.none()) return unknown_tempo_matches ? 1.0 : no_distance;
  float4 ffa = fa.tempo;
  float4 ffb = fb.tempo;
  if (ffa>0 && ffb>0)
    {
      ffa = log(ffa);
      ffb = log(ffb);
    }
  float4 d = ffa - ffb;
  return d / tempo_scale;
}

float4 SongMetriek::spectrum_dist(const Song & self, const Song & song) const 
{
  if (self.get_spectrum()==no_spectrum || song.get_spectrum()==no_spectrum)
    return 1000000;
  spectrum_type *b = song.get_spectrum();
  spectrum_type *a = self.get_spectrum();
  spectrum_freq distance=0;
  for (int i = 0; i < spectrum_size ; i ++ )
    {
      spectrum_freq c = normalize_freq(a->band(i),i);
      spectrum_freq d = normalize_freq(b->band(i),i);
      spectrum_freq mismatch = c-d;
      mismatch*=mismatch;
      distance+=mismatch;
    }
  distance *= 0.01;
  return distance;
}

float4 SongMetriek::histogram_dist(const Song& self, const Song & song, 
				   float8 breakat) const
{
  echo_property b = song.get_histogram();
  echo_property a = self.get_histogram();
  if (a.empty() || b.empty())
    return 1000000;
  float4 distance=0;
  /* recalculate breakat */
  breakat *= (spectrum_size*echo_prop_sx)/100;
  breakat *= breakat;
  /* The distance measure using the differential of the autocorrelation */
  for (int x = 0; x < spectrum_size && distance < breakat; x ++ )
    {
      int as = a.get_count(x);
      int bs = b.get_count(x);
      // compare the common piece
      int cs;
      if (as<=bs) cs=as;
      else cs=bs;
      for(int y = 0 ; y < cs ; y++)
	{
	  float4 c = a.get_freq_energy_probability(x,y)/255.0;
	  // c = normalize_echo(c,x,y);
	  float4 d = b.get_freq_energy_probability(x,y)/255.0;
	  // d = normalize_echo(d,x,y);
	  float4 e = c - d;
	  e *= e;
	  distance+=e;
	}
      // add the remaining
      if (as>cs)
	for(int y = cs ; y < as ; y++)
	  {
	    float4 c = a.get_freq_energy_probability(x,y)/255.0;
	    c *= c;
	    distance+=c;
	  }
      if (bs>cs)
	for(int y = cs ; y < bs ; y++)
	  {
	    float4 d = b.get_freq_energy_probability(x,y)/255.0;
	    d *= d;
	    distance+=d;
	  }
    }
  if (distance>=breakat)
    return 1000000;
  distance = sqrt(distance);
  distance /= (spectrum_size*echo_prop_sx)/100;
  return distance;
}

float4 SongMetriek::rhythm_dist(const Song& self, const Song & song, 
				float8 breakat) const
{
  rhythm_property b = song.get_rhythm();
  rhythm_property a = self.get_rhythm();
  if (a.empty() || b.empty()) return 1000000;
  float4 distance=0;
  // recalculate the breakat argument
  breakat *= (spectrum_size*rhythm_prop_sx)/100;
  breakat *= breakat;
  for (int x = 0; x < spectrum_size && distance < breakat; x ++ )
    for(int y = 0 ; y < rhythm_prop_sx; y++)
      {
	float4 c = a.get_energy(x,y)/255.0;
	if (c<0.5) c=0.5;
	float4 d = b.get_energy(x,y)/255.0;
	if (d<0.5) d=0.5;
	float4 e = c-d;
	e *= e;
	e *= e;
	distance+=e;
      }
  if (distance>=breakat) return 1000000;
  distance = sqrt(distance);
  distance /= (spectrum_size*rhythm_prop_sx)/100;
  return distance /* * Config::get_distance_spectrumscale() */;
}

float4 SongMetriek::composition_dist(const Song & self, const Song & song, 
				     float8 breakat) const
{
  composition_property b = song.get_composition();
  composition_property a = self.get_composition();
  if (a.empty() || b.empty()) return 1000000;
  float4 distance=0;
  // recalculate the breakat argument
  breakat *= (24.0*32.0)/1000.0;
  breakat *= breakat;
  for (int x = 0; x < 24 && distance < breakat; x ++ )
    {
      for(int y = 0 ; y < 32; y++)
	{
	  float4 c = a.get_energy_scaled(x,y);
	  float4 d = b.get_energy_scaled(x,y);
 	  float4 e = (c-d);
	  e*=e;
	  e/=(y+16.0);
	  distance+=e;
	}
    }
  if (distance>=breakat) return 1000000;
  distance = sqrt(distance);
  distance /= (32.0*24.0)/1000.0;
  return distance /* * Config::get_distance_spectrumscale() */;
}

/**
 * These should better be sorted according to the metric. The one with the 
 * highest impact should be measured first. Currently we only do this for the 
 * intensive histogram and rhythm matching.
 */
float4 SongMetriek::distance(const Song &a, const Song &b, float8 limit) const
{
  if (total == 0) return no_distance;
  float4 sum = 0;
  limit*=total;
  float4 breakat=limit;
  if (tempo > 0  && (breakat = limit/tempo) > 0)
    sum+=tempo_dist(a,b) * tempo;
  
  if (spectrum > 0  &&  (breakat = ( limit - sum ) / spectrum) > 0)
    sum+=spectrum_dist(a,b)*spectrum;
  
  if (histogram > rhythm)
    {
      if (histogram > 0 
	  && (breakat = ( limit - sum ) / histogram) > 0)
	sum+=histogram_dist(a,b,breakat) * histogram;
      
      if (rhythm > 0
	  && (breakat= ( limit - sum ) / rhythm)>0)
	sum+=rhythm_dist(a,b,breakat) * rhythm;
    }
  else
    {
      if (rhythm > 0
	  && (breakat= ( limit - sum ) / rhythm)>0)
	sum+=rhythm_dist(a,b,breakat) * rhythm;
      
      if (histogram > 0 
	  && (breakat = ( limit - sum ) / histogram) > 0)
	sum+=histogram_dist(a,b,breakat) * histogram;
    }
  
  if (composition > 0
      && (breakat= ( limit - sum ) / composition) > 0)
    sum+=composition_dist(a,b,breakat) * composition;
  
  return sum/total;
}

SongMetriek SongMetriek::std(0.0,0.3,1.0,2.0,1.0);
//static SongMetriek stddcolor_distance(0.0,0.0,0.0,0.0,1.0);

static float4 calculate_harmonic_term(float4 harmonic, float4 limits)
{
  return log(harmonic)/log(1+limits);
}

void SongMetriek::set_show_harmonics(bool h5, bool h6,bool h7,bool h8)
{
  harmonic545 = h5;
  harmonic646 = h6;
  harmonic747 = h7;
  harmonic848 = h8;
}

void SongMetriek::prepare()
{
  total = tempo + spectrum + histogram + rhythm + composition;
  if (total<1.0) total = 1.0;
  tempo_scale = log(1+get_tempo_limits());
  harmonic54_term=calculate_harmonic_term(5.0/4.0,get_tempo_limits());
  harmonic64_term=calculate_harmonic_term(6.0/4.0,get_tempo_limits());
  harmonic74_term=calculate_harmonic_term(7.0/4.0,get_tempo_limits());
  harmonic84_term=calculate_harmonic_term(8.0/4.0,get_tempo_limits());
  harmonic45_term=calculate_harmonic_term(4.0/5.0,get_tempo_limits());
  harmonic46_term=calculate_harmonic_term(4.0/6.0,get_tempo_limits());
  harmonic47_term=calculate_harmonic_term(4.0/7.0,get_tempo_limits());
  harmonic48_term=calculate_harmonic_term(4.0/8.0,get_tempo_limits());
}
#endif // __loaded__song_metric_cpp__
