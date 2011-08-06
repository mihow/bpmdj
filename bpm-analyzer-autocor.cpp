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
#ifndef __loaded__bpm_analyzer_autocor_cpp__
#define __loaded__bpm_analyzer_autocor_cpp__
using namespace std;
#include <math.h>
#include <assert.h>
#include <fftw3.h>
#include "signals.h"
#include "bpm-analyzer-autocor.h"

BpmAnalyzerAutocor::BpmAnalyzerAutocor(stereo_sample2 *block, unsigned4 n, 
				       float8 lower_boundary, float8 higher_boundary): 
  BpmAnalyzerImpl(44100,true),
  spectrum_shifter(2), audiorate(11025)
{
  input_audio=block;
  input_samples=n;
  startbpm=lower_boundary;
  stopbpm=higher_boundary;
  freq = NULL;
  audio = NULL;
}

fft_type BpmAnalyzerAutocor::index2autocortempo(signed4 i)
{
  assert(i);
  fft_type measure_period_in_ticks = i<<spectrum_shifter;
  fft_type measure_period_in_secs  = measure_period_in_ticks/(fft_type)samplerate;
  fft_type measure_frequency_in_hz = 1/measure_period_in_secs;
  fft_type measure_frequency_in_bpm = measure_frequency_in_hz*60.0;
  fft_type beat_frequency_in_bpm = measure_frequency_in_bpm*4.0;
  return beat_frequency_in_bpm;
}

bool BpmAnalyzerAutocor::step()
{
  signed8 audiosize = input_samples;
  // OK, nu moeten we dat delen door 2^spectrum_shifter
  audiosize>>=spectrum_shifter;
  windowsize = lower_power_of_two(audiosize);
  status("Downscaling audio");
  signed4 blocksize = 1 << spectrum_shifter;
  stereo_sample2 *block = bpmdj_allocate(blocksize,stereo_sample2);
  fft_type *audio = (fft_type*)fftw_malloc(audiosize*sizeof(fft_type));
  fftw_complex* freq_tmp = (fftw_complex*)fftw_malloc(windowsize*
						      sizeof(fftw_complex));
  fftw_plan forward_plan = fftw_plan_dft_r2c_1d(windowsize,audio,freq_tmp,
						FFTW_ESTIMATE);
  freq = (fft_type*)fftw_malloc(windowsize*sizeof(fft_type));
  fftw_plan backward_plan = fftw_plan_dft_c2r_1d(windowsize,freq_tmp,freq,
						 FFTW_ESTIMATE);
  for(signed4 i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      memcpy(block,input_audio+i*blocksize,sizeof(stereo_sample2)*blocksize);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=block[j].left;
      sum/=blocksize;
      audio[i]=sum;
    }
  
  // 1. do a FFT of the entire sample
  status("Forward FFT");
  fftw_execute(forward_plan);
  fftw_destroy_plan(forward_plan);
  fftw_free(audio);

  // 2. modify freq[i]=norm(freq[i])^2;
  for(signed4 i = 0 ; i < windowsize; i ++)
    {
      freq_tmp[i][0] = freq_tmp[i][0] * freq_tmp[i][0]
	+ freq_tmp[i][1] * freq_tmp[i][1];
      freq_tmp[i][1]=0;
    }
  status("Copied");
  
  // 3. do an inverse Fourier transform of freq[i]
  fftw_execute(backward_plan);
  fftw_destroy_plan(backward_plan);
  fftw_free(freq_tmp);
  status("Backward FFT");
  
  // 4. rescale & find peaks 
  // rescale the entire thing
  fft_type max = 0;
  fft_type min = -1.0;
  for(signed4 i = 1 ; i <windowsize/2 ; i ++)
    {
      fft_type bpm = index2autocortempo(i);
      if (bpm<startbpm) break;
      if (bpm>stopbpm) continue;
      freq[i]=log(fabs(freq[i]));
      // freq[i]=fabs(freq[i]);
      if (freq[i]>max) max=freq[i];
      if (freq[i]<min || min < 0.0) min=freq[i];
    }
  
  status("maximum =%g, minimum = %g",max,min);
  for (signed4 i = 0 ; i < windowsize / 2 ; i ++)
    freq[i]=(freq[i]-min)/(max-min);
  
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
      for(signed4 i = 1 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = index2autocortempo(i);
	  
	  // skip or break ?
	  if (bpm<startbpm) break;
	  if (bpm>stopbpm) continue;
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
      if (j == 0)
	tempo=at;
      status("Peak %d at %g with strength %g",j,at,energy);
      
      // clear neighbors
      for(signed4 i = 1 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = index2autocortempo(i);
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm<at-range) break;
	}
    }

  // draw the content 
  assert(freq);
  if (plot) 
    {
      // first find the upper and lower bounds of the energy spectrum
      float8 min_energy=1.0;
      float8 max_energy=0.0;
      for(signed4 i = 0 ; i < windowsize/2 ; i ++)
	{
	  if (freq[i]>max_energy)
	    max_energy=freq[i];
	  if (freq[i]<min_energy)
	    min_energy=freq[i];
	}
      max_energy-=min_energy;      
      axes(0,horizontal(),axis("relative value"),"Amplitude Autocorrelation");
      for(signed4 i = 1 ; i <windowsize/2; i ++)
	period_point(0,i<<spectrum_shifter,freq[i]);
      /*  for (signed4 i = 0 ; i < peaks ; i ++)
	  {
	  QColor c(255-255*i/peaks,0,0);
	  p.setPen(c);
	  float8 bpm = peak_bpm[i];
	  float8 energy = peak_energy[i];
	  signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
	  signed4 y = (signed4)((float8)ys-(float8)ys*energy);
	  QString text = QString::number(i)+") "+QString::number(bpm);
	  p.drawText(x,y,text);
	  }
      */
    }
  finished();
  return false;
}
#endif // __loaded__bpm_analyzer_autocor_cpp__
