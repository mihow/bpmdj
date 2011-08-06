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
#ifndef __loaded__bpm_analyzer_env_cpp__
#define __loaded__bpm_analyzer_env_cpp__
using namespace std;
#include <assert.h>
#include <math.h>
#include <fftw3.h>
#include "signals.h"
#include "bpm-analyzer-env.h"

BpmAnalyzerEnv::BpmAnalyzerEnv(stereo_sample2 *block, unsigned4 n, 
			       float8 lower_boundary, float8 higher_boundary): 
  BpmAnalyzerImpl(44100,true), audiorate(11025), spectrum_shifter(2)
{
  input_audio=block;
  input_samples=n;
  startbpm=lower_boundary;
  stopbpm=higher_boundary;
  startshift = 0;
  stopshift = 0;
  freq = NULL;
  audio = NULL;
}

bool BpmAnalyzerEnv::step()
{
  status("Reading audio");
  signed8 audiosize=input_samples;
  audiosize>>=spectrum_shifter;
  windowsize=lower_power_of_two(audiosize);
  signed4 blocksize = 1 << spectrum_shifter;
  stereo_sample2 *block = bpmdj_allocate(blocksize,stereo_sample2);
  fft_type *audio = (fft_type*)fftw_malloc(audiosize*sizeof(fft_type));
  freq = (fft_type*)fftw_malloc(windowsize*sizeof(fft_type));
  fftw_plan plan = fftw_plan_r2r_1d(windowsize,audio,freq,FFTW_R2HC,
				    FFTW_ESTIMATE);
  for(signed4 i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      memcpy(block,input_audio+i*blocksize,sizeof(stereo_sample2)*blocksize);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].left);
      sum/=blocksize;
      audio[i]=sum;
    }
  status("FFT");
  fftw_execute(plan);
  // rescale the entire thing
  for(signed4 i = 0 ; i <windowsize/2 ; i ++)
    {
      fft_type bpm = Index_to_frequency(windowsize,i); // tov samplerate
      bpm*=(fft_type)samplerate;      // in Hz tov non collapsed samplerate
      // uitgedrukt in collapsed samplerate
      for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0; 
      bpm*=60.0;                                      // uitgedrukt in BPM.
      if (bpm<startbpm) continue;
      if (bpm>stopbpm) break;
      
      float8 re=freq[i];
      float8 im=freq[windowsize-1-i];
      freq[i]=log(sqrt(re*re+im*im));
      // freq[i]=fabs(freq[i]);
    }  

  // detect peak bpm's
  peaks = 10;
  peak_bpm = bpmdj_allocate(peaks, fft_type);
  peak_energy = bpmdj_allocate(peaks, fft_type);
  fft_type *copy = bpmdj_allocate(windowsize / 2, fft_type);
  for(signed4 i = 0 ; i < windowsize/2 ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // BPM left and right...
  
  for(signed4 j = 0 ; j < peaks ; j ++)
    {
      fft_type  energy = 0, at = 0;
      for(signed4 i = 0 ; i <windowsize/2 ; i ++)
	{
	  // tov samplerate
	  fft_type bpm = Index_to_frequency(windowsize,i); 
	  // in Hz tov non collapsed samplerate
	  bpm*=(fft_type)samplerate; 
	  // uitgedrukt in collapsed samplerate
	  for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;
	  bpm*=60.0;                                      // uitgedrukt in BPM.
	  // skip or break ?
	  if (bpm<startbpm) continue;
	  if (bpm>stopbpm) break;
	  // is larger than any of the known peaks ?
	  if (copy[i]>energy)
	    {
	      energy = copy[i];
	      at = bpm;
	    }
	}
      
      // store peak
      peak_bpm[j]=at;
      peak_energy[j]=energy;
      status("Peak %d at %g with strength %g",j,at,energy);
      if (j == 0) 
	tempo=at;
      // clear neighbors
      for(signed4 i = 0 ; i <windowsize/2 ; i ++)
	{
	  // obtain BPM
	  // relatief tov samplerate
	  fft_type bpm = Index_to_frequency(windowsize,i);
	  bpm*=(float8)samplerate;             // in Hz tov non collapsed samplerate
	  // in collapsed samplerate
	  for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;
	  bpm*=60.0;                                      // in BPM.
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  assert(freq);
  if (plot)
    {
      axes(0,horizontal(),axis("relative value"),"HalfReal FFTW Envelope Spectrum");
      for(signed4 i = 1 ; i <windowsize/2; i ++)
	if (freq[i]>0)
	  period_point(0,(samplerate*(double)windowsize/(double)(i*audiorate))/4.,freq[i]);
    }
  fftw_destroy_plan(plan);
  fftw_free(audio);
  finished();
  return false;
}
#endif // __loaded__bpm_analyzer_env_cpp__
