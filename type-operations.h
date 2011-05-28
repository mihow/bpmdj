/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__type_operations_h__
#define __loaded__type_operations_h__
using namespace std;
#line 1 "type-operations.h++"
#include "tempo-type.h"
#include "period-type.h"
#include "stereo-sample2.h"
#include "histogram-type.h"
#include "sample4-type.h"
#include "power-type.h"
#include "spectrum-type.h"
#include "tag-type.h"

/**
 * period_type - describes the length of a period in the index file, 
 *               normalized to metarate (44100 Hz normally)
 * quad_period_type - describes the length of a measure in the player, 
 *                    depending on different wavrates
 * tempo_type - describes the BPM of the song, given a period_type
 */

inline tempo_type period_to_tempo(period_type a)
{
  tempo_type b;
  if (a.period > 0)
    b.tempo = 4.0*11025.0*60.0/(float8)a.period;
  return b;
}

inline period_type tempo_to_period(tempo_type a)
{
  period_type b;
  if (a.tempo > 0)
    b.period = (signed4)(4.0*11025.0*60.0/(float8)a.tempo);
  return b;
}

inline quad_period_type period_to_quad(period_type a)
{
  return quad_period_type(a.period * 4);
}

inline tempo_type between_tempos(tempo_type a, tempo_type b, float4 percent)
{
  return tempo_type((b.tempo-a.tempo)*percent+a.tempo);
}

inline spectrum_type * between_spectra(spectrum_type *a, spectrum_type *b,
				       float4 percent)
{
  if (a==no_spectrum) return no_spectrum;
  if (b==no_spectrum) return no_spectrum;
  spectrum_type * result = new spectrum_type();
  for (signed4 i = 0; i < spectrum_size ; i++)
    result->set_band(i,a->band(i)*percent + b->band(i)*(1.0-percent));
  return result;
}

inline signed4 compare_tempo(tempo_type a, tempo_type b)
{
  if (a.tempo > b.tempo) return 1;
  if (a.tempo < b.tempo) return -1;
  return 0;
}
#endif // __loaded__type_operations_h__
