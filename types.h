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

#ifndef TYPES_BH
#define TYPES_BH

#include "tempo-type.h"
#include "period-type.h"
#include "stereo-sample2.h"
#include "histogram-type.h"
#include "synced-stereo-sample2.h"
#include "sample4-type.h"
#include "power-type.h"
#include "spectrum-type.h"
#include "tag-type.h"

// period_type      - describes the length of a period in the index file, normalized to 44100 Hz
// quad_period_type - describes the length of a measure in the player, depending on different wavrates
// tempo_type       - describes the BPM of the song, given a period_type

inline tempo_type period_to_tempo(period_type a)
{
  tempo_type b;
  if (a.period > 0)
    b.tempo = 4.0*11025.0*60.0/(double)a.period;
  return b;
}

inline period_type tempo_to_period(tempo_type a)
{
  period_type b;
  if (a.tempo > 0)
    b.period = (int)(4.0*11025.0*60.0/(double)a.tempo);
  return b;
}

inline quad_period_type period_to_quad(period_type a)
{
  return quad_period_type(a.period * 4);
}

inline tempo_type between_tempos(tempo_type a, tempo_type b, float percent)
{
  return tempo_type((b.tempo-a.tempo)*percent+a.tempo);
}

inline spectrum_type * between_spectra(spectrum_type *a, spectrum_type *b, float percent)
{
  if (a==no_spectrum) return no_spectrum;
  if (b==no_spectrum) return no_spectrum;
  spectrum_type * result = new spectrum_type();
  for (int i = 0; i < spectrum_size ; i++)
    result->set_band(i,a->band(i)*percent + b->band(i)*(1.0-percent));
  return result;
}

inline int compare_tempo(tempo_type a, tempo_type b)
{
  if (a.tempo > b.tempo) return 1;
  if (a.tempo < b.tempo) return -1;
  return 0;
}

#endif 

