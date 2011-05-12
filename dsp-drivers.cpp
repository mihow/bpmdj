/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle

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

#include "dsp-oss.h"
#include "dsp-alsa.h"

#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#error -------------------------------------------
#error Should at least compile one dsp driver !!! 
#error Try using -D COMPILE_OSS or -D COMPILE_ALSA
#error -------------------------------------------
#endif
#endif

// dispatching should be redone in C++
int dsp_oss = 1;
int dsp_alsa = 2;
int dsp_driver = 1;

#ifdef COMPILE_OSS
#define OSS(A) if (dsp_driver == dsp_oss) A;
#else
#define OSS(A) ;
#endif

#ifdef COMPILE_ALSA
#define ALSA(B) if (dsp_driver == dsp_alsa) B;
#else
#define ALSA(B) ;
#endif

/*-------------------------------------------
 *         Dsp dispatching
 *-------------------------------------------*/ 
void dsp_start()
{
  OSS(oss_start());
  ALSA(alsa_start());
}

void dsp_pause()
{
  OSS( oss_pause());
  ALSA( alsa_pause());
}

void dsp_write(unsigned4 *value)
{
  OSS( oss_write(value));
  ALSA( alsa_write(value));
}

signed8 dsp_latency()
{
  OSS( return oss_latency());
  ALSA( return alsa_latency());
}

int dsp_open()
{
  OSS( return oss_open() );
  ALSA( return alsa_open() );
  return -10;
}

void dsp_close()
{
  OSS(oss_close());
  ALSA(alsa_close());
}
