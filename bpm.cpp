/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__bpm_cpp__
#define __loaded__bpm_cpp__
using namespace std;
#line 1 "bpm.c++"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include "bpm.h"
#include "memory.h"
#include "signals.h"

typedef stereo_sample2 sample_type;

void BpmCounter::init(unsigned4 slen, sample_type *blk, int inrate, 
float8 lower_boundary, float8 higher_boundary)
{
  /** 
   * We calculate the length of an analysis block. We aim to include all 
   * frequencies starting from at least 50 Hz. If we start at 44100 Hz we must
   * halve the value 11 times. Which means that we must have at least a buffer
   * of 1<<11
   */
  block=blk;
  input_rate = inrate;
  measure_rate = 11025;
  sample_length = slen;
  unsigned4 atmost = (signed8)sample_length*measure_rate/inrate;
  measured = 1;
  while(measured <= atmost) measured *=2;
  measured/=2;
  audio = bpmdj_allocate(measured,float8);
  
  /**
   * Preparation of the Fourier windows, for every band we will have a separate
   * plan available. All output is stored in the energy array, which is then 
   * used to compute the autocorrelation sequence
   */
  FILE * wisdom_file = fopen(".bpmdj_fftw_wisdom","rb");
  if (wisdom_file)
    {      
      if (log) fprintf(log,"Loading fftw wisdom\n");
      fftw_import_wisdom_from_file(wisdom_file);
      fclose(wisdom_file);
    }
  else
    {
      if (log) 
	fprintf(log,"Information: The first analysis might take a long time "
		"(>15 minutes) due to calibration of the fftw lib\n");
    }
  
  en = bpmdj_allocate(measured, float8);
  ts = bpmdj_allocate(measured, float8);
  co = bpmdj_allocate(measured, float8);
  
  if (log) fprintf(log,"Preparing Fourier transform (%d)\n",measured);
  forward = fftw_plan_r2r_1d(measured, &(audio[0]), en, FFTW_R2HC, 
			     FFTW_MEASURE);
  backward = fftw_plan_r2r_1d(measured, ts, co, FFTW_HC2R, 
			      FFTW_MEASURE);
  
  wisdom_file = fopen(".bpmdj_fftw_wisdom","wb");
  if (wisdom_file)
    {
      if (log) fprintf(log,"Saving fftw wisdom\n");
      fftw_export_wisdom_to_file(wisdom_file);
      fclose(wisdom_file);
    }
  
  float8 freq = higher_boundary/60.0;
  b1 = 4 * (int)(measure_rate / freq);
  freq = lower_boundary/60.0;
  b2 = 4 * (int)(measure_rate / freq);
  if (log) fprintf(log,"Search boundaries are %d to %d\n",b1,b2);
}

BpmCounter::~BpmCounter()
{
  bpmdj_deallocate(en);
  bpmdj_deallocate(co);
  bpmdj_deallocate(ts);
}

float8 BpmCounter::measure()
{
  // 1. Copy & resample
  if (log) fprintf(log,"Reading Audio\n");
  assert(block);
  for(signed8 i = 0 ; i < measured ; i ++)
    {
      signed8 posa = i*input_rate/measure_rate;
      signed8 posb = (i+1)*input_rate/measure_rate;
      float8 sum = 0;
      if (posb>=sample_length) posb=sample_length;
      for(unsigned4 j = posa; j< posb; j++)
	sum += fabs(block[j].left)+fabs(block[j].right);
      audio[i]=sum/(2*(posb-posa));
    }
  // 2. Energy
  if (log) fprintf(log,"Measuring Energy\n");
  int blsi = 1;
  int tmpr = measure_rate;
  while(tmpr>50) 
    {
      blsi*=2;
      tmpr/=2;
    }
  energize(audio,measured,blsi);

  float8 * w = bpmdj_allocate(measured,float8);
  for(unsigned int x = 0 ; x < measured ; x ++)
    w[x]=audio[x];
  differentiate(w,measured);
  normalize_abs_max(w,measured);
  normalize_abs_max(audio,measured);
  for(unsigned int x = 0 ; x < measured ; x ++)
    audio[x]+=w[x];
  bpmdj_deallocate(w);
  
  // 3. Mask
  if (log) fprintf(log,"Masking\n");
  for(unsigned4 x = 0 ; x < measured ; x ++)
    audio[x]*= 0.5 - 0.5 * cos(M_PI*2.0*(float8)x/(float8)measured);
  
  // 4. Forward transform
  if (log) fprintf(log,"Forward transform\n");
  fftw_execute(forward);
  
  // 5. Measure energy
  if (log) fprintf(log,"Frequency Content\n");
  for(unsigned4 x = 1 ; x < measured / 2; x ++)
    en[x]=sqrt(en[x]*en[x]+en[measured-x]*en[measured-x]);
  for(unsigned4 x = 1 ; x < measured / 2; x ++)
    en[x]/=measured/x;
  en[0]=0.0;
  for(unsigned4 x = 0 ; x < measured / 2; x ++)
    ts[x]=en[x];
  
  // 6. Zero pad & backward transform
  for(unsigned4 x = measured / 2 ; x < measured ; x ++)
    ts[x]=0;
  if (log) fprintf(log,"Autocorrelate\n");
  fftw_execute(backward);
  
  // 8. Normalize the autocorrelation
  if (log) fprintf(log,"Normalize\n");
  for(unsigned4 x = 1 ; x < measured ; x ++)
    co[x]/=measured-x;
  co[0]=0;

  // 9. Combine and find best match
  if (log) fprintf(log,"Find tempo\n");
  float8 mf = en[0] = 0;
  int pf = 0;
  normalize_abs_max(en,measured);
  normalize_abs_max(co,measured);

  // Dump
  /*  printf("\n");
  for(long x = measured/2 ; x > 1 ; x--)
    {
      float8 freq = 44100.0/(float8)x;
      printf("%g %g\n",freq,en[x]);
      if (freq>4.0) break;
    }
  printf("\n");
  */

  for(unsigned4 x = 1 ;  x < measured/2 ; x ++)
    co[x]*=en[measured/x];
  for(unsigned4 x = b1 ;  x < b2 ; x ++)
    {
      int m = 1;
      float8 E = 0;
      while (m<=2)
	{
	  E += co[x*m];
	  E += co[x/m];
	  m *= 2;
	}
      if (E>mf || mf == 0.0)
	{
	  mf = E;
	  pf = x;
	}
    }
  
  // 10. Return tempo
  float8 t = 4.0 * measure_rate * 60.0 / (float8) pf ;
  if (log) fprintf(log,"Tempo = %g\n",t);
  return t;
}
#endif // __loaded__bpm_cpp__
