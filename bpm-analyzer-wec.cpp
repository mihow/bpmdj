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
#ifndef __loaded__bpm_analyzer_wec_cpp__
#define __loaded__bpm_analyzer_wec_cpp__
using namespace std;
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <fftw3.h>
#include "bpm-analyzer-wec.h"
#include "memory.h"
#include "signals.h"

typedef stereo_sample2 sample_type;

/* replaces every element with the RMS value of 
 * the surrounding block of size ws */
void energize(float8*data, float8 audiosize, unsigned4 fs)
{
  assert(fs>0);
  bpmdj_array(rr,fs,float4);
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  float8 M = 0;
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    {
      M-=rr[x%fs];
      rr[x%fs]=data[x];
      M+=data[x];
      float8 R = M/fs;
      data[x>=fs ? x - fs : 0 ] -= R;
    }
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    data[x]*=data[x];
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  float8 S = 0;
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    {
      S-=rr[x%fs];
      rr[x%fs]=data[x];
      S+=data[x];
      assert(S>=0);
      float8 R = sqrt(S/fs);
      assert(!isnan(R));
      data[x>=fs ? x - fs : 0 ] = R;
    }
  bpmdj_deallocate(rr);
}

void BpmAnalyzerWec::init()
{
  /** 
   * We calculate the length of an analysis block. We aim to include all 
   * frequencies starting from at least 50 Hz. If we start at 44100 Hz we must
   * halve the value 11 times. Which means that we must have at least a buffer
   * of 1<<11
   */
  unsigned4 atmost = (signed8)input_samples*measure_rate/samplerate;
  measured = 1;
  while(measured <= atmost) measured *=2;
  measured/=2;
  audio = bpmdj_allocate(measured,float8);  
  en = bpmdj_allocate(measured, float8);
  ts = bpmdj_allocate(measured, float8);
  co = bpmdj_allocate(measured, float8);
}

BpmAnalyzerWec::~BpmAnalyzerWec()
{
  bpmdj_deallocate(en);
  bpmdj_deallocate(co);
  bpmdj_deallocate(ts);
}

bool BpmAnalyzerWec::step()
{
  float8 freq = (float8)stopbpm/60.0;
  b1 = 4 * (int)(measure_rate / freq);
  freq = (float8)startbpm/60.0;
  b2 = 4 * (int)(measure_rate / freq);
  status("Search boundaries are %d to %d",b1,b2);
  status("Preparing Fourier transform (%d)",measured);
  /**
   * Preparation of the Fourier windows, for every band we will have a separate
   * plan available. All output is stored in the energy array, which is then 
   * used to compute the autocorrelation sequence
   */
  FILE * wisdom_file = fopen(".bpmdj_fftw_wisdom","rb");
  if (wisdom_file)
    {      
      status("Loading fftw wisdom");
      fftw_import_wisdom_from_file(wisdom_file);
      fclose(wisdom_file);
    }
  else
    status("The first analysis might take a long time "
	   "(>15 minutes) due to calibration of the fftw lib");
  forward = fftw_plan_r2r_1d(measured,&(audio[0]), en, FFTW_R2HC, FFTW_ESTIMATE);
  backward = fftw_plan_r2r_1d(measured, ts, co, FFTW_HC2R, FFTW_ESTIMATE);
  
  wisdom_file = fopen(".bpmdj_fftw_wisdom","wb");
  if (wisdom_file)
    {
      status("Saving fftw wisdom");
      fftw_export_wisdom_to_file(wisdom_file);
      fclose(wisdom_file);
    }
  
  // 1. Copy & resample
  status("Reading Audio");
  assert(input_audio);
  for(signed8 i = 0 ; i < measured ; i ++)
    {
      signed8 posa = i*samplerate/measure_rate;
      signed8 posb = (i+1)*samplerate/measure_rate;
      assert(posa!=posb);
      float8 sum = 0;
      if (posb>=input_samples) posb=input_samples;
      for(unsigned4 j = posa; j< posb; j++)
	sum += fabs(input_audio[j].left)+fabs(input_audio[j].right);
      audio[i]=sum/(2*(posb-posa));
      assert(!isnan(audio[i]));
    }
  // 2. Energy
  status("Measuring Energy");
  int blsi = 1;
  int tmpr = measure_rate;
  while(tmpr>50) 
    {
      blsi*=2;
      tmpr/=2;
    }
  energize(audio,measured,blsi);

  float8 * w = bpmdj_allocate(measured,float8);
  assert(w);
  for(unsigned int x = 0 ; x < measured ; x ++)
      w[x]=audio[x];
  differentiate(w,measured);
  normalize_abs_max(w,measured);
  normalize_abs_max(audio,measured);
  for(unsigned int x = 0 ; x < measured ; x ++)
    audio[x]+=w[x];
  bpmdj_deallocate(w);
  
  // 3. Mask
  status("Masking");
  for(unsigned4 x = 0 ; x < measured ; x ++)
    audio[x]*= 0.5 - 0.5 * cos(M_PI*2.0*(float8)x/(float8)measured);
  
  // 4. Forward transform
  status("Forward transform");
  fftw_execute(forward);
  
  // 5. Measure energy
  status("Frequency Content");
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
  status("Autocorrelate");
  fftw_execute(backward);
  
  // 8. Normalize the autocorrelation
  status("Normalize");
  for(unsigned4 x = 1 ; x < measured ; x ++)
    co[x]/=measured-x;
  co[0]=0;

  // 9. Combine and find best match
  status("Find tempo");
  float8 mf = en[0] = 0;
  int pf = 0;
  normalize_abs_max(en,measured);
  normalize_abs_max(co,measured);
 
  for(unsigned4 x = 1 ;  x < measured/2 ; x ++)
    co[x]*=en[measured/x];
  axes(0,horizontal(),axis("relative values"), "Autocorrelation & Energy Envelope");
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
      period_point(0,(float8)x*(float8)samplerate/(float8)measure_rate,E);
    }
  
  // 10. Return tempo
  tempo = 4.0 * measure_rate * 60.0 / (float8) pf ;
  status("Tempo = %g",tempo);
  finished();
  return false; 
}
#endif // __loaded__bpm_analyzer_wec_cpp__
