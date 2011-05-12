/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>
#include <qspinbox.h>
#include <sys/stat.h>
#include <qpainter.h>
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "history.h"
#include "dirscanner.h"
#include "spectrum-type.h"
#include "composition-property.h"
#include "tags.h"
#include "memory.h"
#include "song-statistics.h"
#include "song-metric.h"
#include "metric-widget.h"

SongMetriek::SongMetriek(float tw, float sw, float hw, float rw, float cw)
{
  tempo = tw;
  spectrum = sw;
  histogram = hw;
  rythm = rw;
  composition = cw;
  harmonic545 = harmonic646 = harmonic747 = harmonic848 = false;
  unknown_tempo_matches = false;
};

SongMetriek::SongMetriek(MetricWidget & take_from)
{
  set_tempo_limits(std.get_tempo_limits());
  set_show_harmonics(std.harmonic54_term, std.harmonic64_term, std.harmonic74_term, std.harmonic84_term);
  unknown_tempo_matches = false;
  
  set_tempo_weight(    (float)(take_from.tempoDistanceSpin->value())/100.0 );
  set_spectrum_weight( (float)(take_from.spectrumSpin->value())/100.0);
  set_echo_weight(     (float)(take_from.echoSpin->value())/100.0);
  set_rythm_weight(    (float)(take_from.rythmSpin->value())/100.0);
  std.set_composition_weight( (float)(take_from.compositionSpin->value())/100.0);
  
  prepare();
}

float SongMetriek::find_harmonic(float td, int & which) const
{
  float d;
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

float SongMetriek::tempo_diff(const Song& a, const Song& b) const
{
  tempo_type fa = a.get_tempo();
  tempo_type fb = b.get_tempo();
  if (fa.none() || fb.none()) return unknown_tempo_matches ? 1.0 : no_distance;
  float ffa = fa.tempo;
  float ffb = fb.tempo;
  if (ffa>0 && ffb>0)
    {
      ffa = log(ffa);
      ffb = log(ffb);
    }
  float d = ffa - ffb;
  return d / tempo_scale;
}

float SongMetriek::spectrum_dist(const Song & self, const Song & song) const 
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

float SongMetriek::histogram_dist(const Song& self, const Song & song, double breakat) const
{
  histogram_property b = song.get_histogram();
  histogram_property a = self.get_histogram();
  if (a.empty() || b.empty())
    return 1000000;
  float distance=0;
  /* recalculate breakat */
  breakat /= 10;
  breakat *= (spectrum_size*smallhistogram_size)/100;
  breakat *= breakat;
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
	  float c = a.get_freq_energy_probability(x,y)/255.0;
	  // c = normalize_echo(c,x,y);
	  float d = b.get_freq_energy_probability(x,y)/255.0;
	  // d = normalize_echo(d,x,y);
	  float e = c - d;
	  e *= e;
	  distance+=e;
	}
      // add the remaining
      if (as>cs)
	for(int y = cs ; y < as ; y++)
	  {
	    float c = a.get_freq_energy_probability(x,y)/255.0;
	    c *= c;
	    distance+=c;
	  }
      if (bs>cs)
	for(int y = cs ; y < bs ; y++)
	  {
	    float d = b.get_freq_energy_probability(x,y)/255.0;
	    d *= d;
	    distance+=d;
	  }
    }
  if (distance>=breakat) 
    return 1000000;
  distance = sqrt(distance);
  distance = sqrt(distance);
  distance /= (spectrum_size*smallhistogram_size)/100;
  distance *= 10;
  return distance;
}

float SongMetriek::rythm_dist(const Song& self, const Song & song, double breakat) const
{
  rythm_property b = song.get_rythm();
  rythm_property a = self.get_rythm();
  if (a.empty() || b.empty()) return 1000000;
  float distance=0;
  // recalculate the breakat argument
  breakat *= (spectrum_size*smallhistogram_size)/100;
  breakat *= breakat;
  for (int x = 0; x < spectrum_size && distance < breakat; x ++ )
    for(int y = 0 ; y < smallhistogram_size; y++)
      {
	float c = a.get_energy(x,y)/255.0;
	if (c<0.5) c=0.5;
	float d = b.get_energy(x,y)/255.0;
	if (d<0.5) d=0.5;
	float e = c-d;
	e *= e;
	e *= e;
	distance+=e;
      }
  if (distance>=breakat) return 1000000;
  distance = sqrt(distance);
  distance /= (spectrum_size*smallhistogram_size)/100;
  return distance /* * Config::get_distance_spectrumscale() */;
}

float SongMetriek::composition_dist(const Song & self, const Song & song, double breakat) const
{
  composition_property b = song.get_composition();
  composition_property a = self.get_composition();
  if (a.empty() || b.empty()) return 1000000;
  float distance=0;
  // recalculate the breakat argument
  breakat *= (24.0*32.0)/1000.0;
  breakat *= breakat;
  for (int x = 0; x < 24 && distance < breakat; x ++ )
    {
      for(int y = 0 ; y < 32; y++)
	{
	  float c = a.get_energy_scaled(x,y);
	  float d = b.get_energy_scaled(x,y);
 	  float e = (c-d);
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
 * These should better be sorted according to the metric. The one with the highest
 * impact should be measured first. Currenlty we only do this for the intensive 
 * histogram and ryhtm matchings.
 */
float SongMetriek::distance(const Song &a, const Song &b, double limit) const
{
  if (total == 0) return no_distance;
  float sum = 0;
  limit*=total;
  float breakat=limit;
  if (tempo > 0  && (breakat = limit/tempo) > 0)
    sum+=tempo_dist(a,b) * tempo;
  
  if (spectrum > 0  &&  (breakat = ( limit - sum ) / spectrum) > 0)
    sum+=spectrum_dist(a,b)*spectrum;
  
  if (histogram > rythm)
    {
      if (histogram > 0 
	  && (breakat = ( limit - sum ) / histogram) > 0)
	sum+=histogram_dist(a,b,breakat) * histogram;
      
      if (rythm > 0
	  && (breakat= ( limit - sum ) / rythm)>0)
	sum+=rythm_dist(a,b,breakat) * rythm;
    }
  else
    {
      if (rythm > 0
	  && (breakat= ( limit - sum ) / rythm)>0)
	sum+=rythm_dist(a,b,breakat) * rythm;
      
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

static float calculate_harmonic_term(float harmonic, float limits)
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
  total = tempo + spectrum + histogram + rythm + composition;
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

