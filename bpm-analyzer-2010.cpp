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
#ifndef __loaded__bpm_analyzer_2010_cpp__
#define __loaded__bpm_analyzer_2010_cpp__
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <sys/mman.h>
#include <syscall.h>
#include "bpm-analyzer-2010.h"
#include "sys/times.h"
#include "version.h"
#include "scripts.h"
#include "memory.h"

BpmAnalyzer2010::BpmAnalyzer2010(stereo_sample2* audio, int n, int samplerate, float from_tempo, float to_tempo): 
  BpmAnalyzerImpl(samplerate,true)
{
  input_audio=audio;
  input_samples=n;
  startbpm = from_tempo;
  stopbpm = to_tempo;
}

#define DEBUG_EXPALG

/**
 * FIT_CURVES will set the analyzer such that it uses a log function to normalize the resulting mismatchgraph
 * This feature improves the output of the algortihm from 30% to 40% successrate on a particular dataset.
 */
#define FIT_CURVES 

/**
 * FFT MIN_SINK will look at the melody spectrum and lower each value based on the last minimum value.
 */
#define FFT_MIN_SINK

/**
 * To help me overload gnuplot, I export all note and frequency data into a nice large file
 */
//#define OUTPUT_BIGFILE

void BpmAnalyzer2010::map_output(float4* target, int startfreq, int count, double* hcfreq, int winsize)
{
  for(int note=0 ; note < count; note++)
    {
      float freqa=startfreq*pow(2,(float)note/12.0);
      float freqb=startfreq*pow(2,(float)(note+1)/12.0);
      /*
	freq=bin*wavrate/windowsize;
	thus to pick out frequency the bin is given by
	bin=freq*windowsuze/wavrate;
      */
      int bina=freqa*winsize/samplerate;
      int binb=freqb*winsize/samplerate;
      assert(bina<winsize/2);
      assert(binb<winsize/2);
      if (binb==bina) binb=bina+1;
      double result=0;
      for(int bin = bina; bin<binb; bin++)
	{
	  if (bin==0)
	    result+=hcfreq[bin];
	  else
	    {
	      int bin1=bin;
	      int bin2=winsize-1-bin;
	      float a=hcfreq[bin1];
	      float b=hcfreq[bin2];
	      double tmp=sqrt(a*a+b*b);
	      assert(tmp>=0);
	      result+=tmp;
	    }
	}
      result/=binb-bina;
      result/=winsize;
      target[note]=result;
    }
}

/**
 * Target must be spectrum_size long
 */
void BpmAnalyzer2010::spectrum_to_bark(fftw_complex* in, int window_size, double* out)
{
  for(int b = 0 ; b < barksize ; b ++)
    {
      int a = (int)barkbounds[b];
      int c = (int)barkbounds[b+1];
      a *= window_size;
      c *= window_size;
      a /= samplerate;
      c /= samplerate;
      if (c==a)
	{
	  printf("a = %d; b=%d, c=%d\n",a,b,c);
	  assert(c!=a);
	}
      double result=0;
      for(int j = a ; j < c; j++)
	{
	  double r=in[j][0];
	  double i=in[j][1];
	  double m=sqrt(r*r+i*i);
	  if (isnan(m)) m=0;
	  assert(m>=0);
	  result+=m;
	}
      out[b]=result/((c-a)*window_size);
    }
}

// so the crosscorrelation has a bit of a drawback namely that it
// prefers a 1/4th shift in most cases ? Why is that ?
// if the tempo is not measured completely accurately that might 
// be a very natural response. 
bool BpmAnalyzer2010::step()
{
#ifdef DEBUG_EXPALG
  FILE * f;
#endif
  /** 
   * load the full data into memory, barring the stereo information
   * that is not so very interesting to consider
   * We will work with 6 different fourier analyses, at lengts
   * 1024, 2048, 4096, 8192, 16384 and 32768. This means that we will need to read
   */
  const int largest_window=32768;
  const int stride=1024;
  if (input_samples<largest_window) 
    {
      status("Not enough data to work with");
      return false;
    }
  int block_count=1+(input_samples-largest_window)/stride;
  /**
   * Each block contains around 85 channels, which are the notes
   */
  const int channel_count=85;
  float**blocks;
  unsigned1 **intblocks;
  blocks=bpmdj_allocate(block_count,float*);
  intblocks=bpmdj_allocate(block_count,unsigned1*);
  for(int i = 0 ; i < block_count; i++)
    {
      blocks[i]=bpmdj_allocate(channel_count,float);
      intblocks[i]=bpmdj_allocate(channel_count,unsigned1);
    }
  /**
   * Preparation of the windowing information
   */
  const int wcounts=6;
  int window_sizes[wcounts]; int startfreqs[wcounts]; int countfreqs[wcounts]; 
  window_sizes[0]=32768; startfreqs[0]=27.5;  countfreqs[0]=10;
  window_sizes[1]=16384; startfreqs[1]=49.0;  countfreqs[1]=12;
  window_sizes[2]=8192;  startfreqs[2]=98.0;  countfreqs[2]=12;
  window_sizes[3]=4096;  startfreqs[3]=196.0; countfreqs[3]=12;
  window_sizes[4]=2048;  startfreqs[4]=392.0; countfreqs[4]=12;
  window_sizes[5]=1024;  startfreqs[5]=783.0; countfreqs[5]=27;  
  int current_channel=0;
  status("Converting audio to frequency blocks");
  for(int widx=0; widx<wcounts; widx++)
    {
      status("Notes from octave %d/%d",widx,wcounts);
      int window_size=window_sizes[widx];
      double* han=bpmdj_allocate(window_size,double);
      for(int i = 0 ; i < window_size; i++)
	{
	  double x=i*2.0*M_PI/(double)window_size;
	  x-=M_PI/2;
	  han[i]=(sin(x)+1)/2;
	}
      double* audio=(fft_type*)fftw_malloc(window_size*sizeof(double));
      double* hcfreq=(fft_type*)fftw_malloc(window_size*sizeof(double));
      fftw_plan plan = fftw_plan_r2r_1d(window_size,audio,hcfreq,FFTW_R2HC,FFTW_MEASURE);
      assert(current_channel+countfreqs[widx]<=channel_count);
      for(int block = 0 ; block < block_count; block++)
	{
	  progress(block,block_count);
          unsigned8 start=block;
	  start*=stride;
	  assert(start+window_size<=(unsigned8)input_samples);
	  
	  const stereo_sample2* intaudio=input_audio+start;
	  for(int i = 0 ; i < window_size; i++)
	    audio[i]=intaudio[i].summed();
	  for(int i = 0 ; i < window_size; i++)
	    audio[i]*=han[i];
	  // perform fourier transform
	  fftw_execute(plan);
	  // map the fourier bins to the proper channel
	  map_output(blocks[block]+current_channel,     // the target channels
		     startfreqs[widx],countfreqs[widx], // the frequencies to be retrieved
		     hcfreq,window_size);               // the frequency content
	}
      current_channel+=countfreqs[widx];
    }

  status("Converting channels to dB");
  /**
   * So that the rayshooter subtracts values with a sensible meaning will we take
   * the log value of each of the outputs
   */
  for(int i = 0 ; i < block_count; i++)
    {
      float* channel=blocks[i];
      for(int c = 0 ; c < channel_count; c++)
	{
	  float val=channel[c];
	  if (val<1) val=1;
	  val=log(val);
	  channel[c]=val;
	}
    }
     
  /**
   * We scale each channel to fall in the 0..255 
   * range. Because we also want to make sure that the values are comparable accross channels
   * will we only normalize towards the total maximum and not per channel.
   */
  status("Rescaling channels");
  double max=0;
  double min=-1;
  for(int i = 0 ; i < block_count; i++)
    {
      float* channel=blocks[i];
      for(int c = 0 ; c < channel_count; c++)
	{
	  float val=channel[c];
	  if (val>max) max=val;
	  if (val<min || min==-1)
	    min=val;
	}
    }
  max-=min;
  if (max==0) max=1;
  for(int i = 0 ; i < block_count; i++)
    {
      float* channel=blocks[i];
      unsigned1* target=intblocks[i];
      for(int c = 0 ; c < channel_count; c++)
	target[c]=(unsigned1)(255.0*(channel[c]-min)/max);
    }
  
  /**
   * Calculate the frequence content of each channel and store it back into
   * the blocks array.
   */
  status("Calculating enveloppe spectrum");
  double* notes=(fft_type*)fftw_malloc(block_count*sizeof(double));
  assert(notes);
  fftw_complex* hcfreq=(fftw_complex*)fftw_malloc(block_count*sizeof(fftw_complex));
  assert(hcfreq);
  fftw_plan plan = fftw_plan_dft_r2c_1d(block_count,notes,hcfreq,FFTW_ESTIMATE);
  assert(plan);
  for(int c = 0 ; c < channel_count; c++)
    {
      progress(c,channel_count);
      for(int b = 0 ; b < block_count; b++)
	notes[b]=blocks[b][c];
      for(int b = block_count; b < block_count; b++)
	notes[b]=0;
      fftw_execute(plan);
      for(int b = 0 ; b < block_count/2; b++)
	{
	  double r=hcfreq[b][0];
	  double i=hcfreq[b][1];
	  double m=sqrt(r*r+i*i);
	  if (m<0) m=1;
	  double d=log(m);
	  blocks[b][c]=d;
	}
    }
  
  /**
   * Place the various note frequencies all together
   * in the hcfreq array for later use. So that
   * to esnure that we can compare the outputs of the 
   * frequency analysis with the rayshoot analysis, we 
   * place them in the proper period instead of copying the
   * bins literally
   */
  double fitter[block_count];
  for(int i = 0 ; i < block_count; i++)
    fitter[i]=0;
  for(int i = 1 ; i < block_count/2; i++)
    {
      double sum=0;
      for(int c = 0 ; c < channel_count; c++)
	sum+=blocks[i][c];
      sum/=channel_count;
      fitter[i]=sum;
    }

#ifdef FFT_MIN_SINK
  status("Normalzing enveloppe spectrum");
  double bpmperbin=60.0*samplerate/(stride*block_count);
  int fb1=startbpm/bpmperbin;
  int fb2=stopbpm/bpmperbin;
  fb2*=4; 
  /**
   * The curve fitter bases itself on the last observed minimum error
   * and will use that to subtract the relevant data directly.
   */
  double mn=fitter[fb1];
  double mismatch_array_freq[block_count];
  int mismatch_array_freqcnt[block_count];
  for(int i = 0 ; i < block_count; i++)
    {
      mismatch_array_freq[i]=0;
      mismatch_array_freqcnt[i]=0;
    }
  for(int p = fb1 ; p <= fb2; p++)
    {
      double v=fitter[p];
      if (v<mn) mn=v;
      fitter[p]=v-mn;
      /**
       * The 4 below is necessary since bin i in the mismatch_array reflects the
       * time necessary to cover one measure and not 1 beat.
       */
      int target_bin=block_count*4/p;
      if (target_bin>=block_count) continue;
      mismatch_array_freq[target_bin]+=fitter[p];
      mismatch_array_freqcnt[target_bin]++;
    }
  /**
   * The calculated frequency content does not automactically full all 
   * bins. Thos that are 0 didn't have a value set.
   */
  double last=0;
  for(int i = 0 ; i < block_count; i++)
    {
      if (mismatch_array_freqcnt[i])
	{
	  last=mismatch_array_freq[i]/mismatch_array_freqcnt[i];
	  mismatch_array_freq[i]=last;
	}
      else
	mismatch_array_freq[i]=last;
    }
#endif
  
#ifdef OUTPUT_BIGFILE
  status("Dumping melody and frequency file");
  f=fopen("bigfile.txt","wb");
  g=fopen("freq.txt","wb");
  for(int i = 0 ; i < block_count; i++)
    {
      fprintf(f,"%d",i);
      for(int c = 0 ; c < channel_count; c++)
	fprintf(f,"\t%d",intblocks[i][c]);
      
      if (i<block_count/2)
	{
	  fprintf(g,"%g",60.0*i*samplerate/(stride*block_count));
	  for(int c = 0 ; c < channel_count; c++)
	    fprintf(g,"\t%g",blocks[i][c]);
	  fprintf(g,"\n");
	}
      
      fprintf(f,"\n");
    }
  fclose(f);
  fclose(g);
#endif
  
  for(int i = 0 ; i < block_count; i++)
    bpmdj_deallocate(blocks[i]);
  bpmdj_deallocate(blocks);
  status("Autodifferencing");
  
  /**
   * Now we start the rayshooting. Since we already have a relative large blocksize,
   * we can simply scan everyhing I believe
   * We could also print out the tempo for each of the different channels and
   * see which ones should carry more weight.
   */
  unsigned8 mismatch_array[block_count];
  double mismatch_array_dbl[block_count];
  for(int i = 0 ; i < block_count; i++)
    {
      mismatch_array[i]=0;
      mismatch_array_dbl[i]=0;
    }

  /**
   * The start_bpm and stopbpm should be converted to the best matching period
   * which in this case is in blocks of stride size
   */
  int pscan_stop=((samplerate*60.0/startbpm) // samplecount per beat
		  *4                       // samples per bar
		  /stride);                // blocks per bar
  int pscan_start=((samplerate*60.0/stopbpm) // samplecount per beat
		   *4                      // samples per bar
		   /stride);               // blocks per bar
  if(pscan_start>2) pscan_start--;         // ensure we certainly cover the stopbpm;
  pscan_stop++;                            // ensure we conver startpbpm

  /**
   * Todo: print warning if bounds are weird
   */
  if (pscan_stop>=block_count/2) pscan_stop=(block_count/2);

  /**
   * Count the number of rays we will be seeing
   */
  unsigned4 raycount=0;
#if defined(FIT_CURVES)
  for(int p=1;p<pscan_stop;p++)
#else
    for(int p=pscan_start;p<pscan_stop;p++)
#endif
      for(int o=0; o < p; o++)
	{
	  int c=(block_count-o)/p;
	  if (o+c*p>=block_count) c--;
	  int i=o+c*p;
	  assert(i<block_count);
	  i-=p;
	  raycount+=i/p;
	}
  if (raycount==0) raycount=1;
  unsigned4 total_rays=raycount;
  raycount=0;
  
  /**
   * An error that could be made here is to scan until we have no data left. That is a bad idea
   * because we will inevitably introduce new data at higher tempos if we do so, skewing the curve
   * So instead of doing that, we calculate the maximum number of blocks we can reach in all cases.
   * 
   * if we introduce more measurements we tend to introduce more random errors; this results 
   * in a biased output. The random errors we introduce is something we cannot really compensate
   * for at first sight, unless we would be measuring aside from the random error also the 
   * probability that this error occurs, which might just be sufficient to figure out 
   * how certain we can be at that partiular position.
   * so we will be scanning twice.
   */
#if defined(FIT_CURVES)
      for(int p=1;p<pscan_stop;p++)
#else
	for(int p=pscan_start;p<pscan_stop;p++)
#endif
	  {
	    unsigned8 offset_mismatch[p];
	    /**
	     * Calculate the average mismatch for each offset
	     */
	    for(int o = 0; o < p; o++)
	      {
		unsigned8 mismatch=0;
		unsigned8 additions=0;
		int c=(block_count-o)/p;
		if (o+c*p>=block_count) c--;
		int i;
		i=o+c*p;
		assert(i<block_count);
		i-=p;
		while(i>=0)
		  {
		    unsigned1* I=intblocks[i];
		    unsigned1* J=intblocks[i+p];
		    int C=channel_count;
		    while(--C>=0)
		      mismatch+=abs((int)I[C]-(int)J[C]);
		    additions++;
		    raycount++;
		    i-=p;
		  }
		progress(raycount,total_rays);
		offset_mismatch[o]=mismatch/additions;
	      }
	    /**
	     * Calculate the deviation for the offset_mismatch_array;
	     */
	    unsigned8 mismatch=0;
	    unsigned8 mismatch_min=offset_mismatch[0];
	    for(int o=0;o<p; o++)
	      {
		unsigned8 v=offset_mismatch[o];
		mismatch+=v;
		if (v<mismatch_min) mismatch_min=v;
	      }
	    mismatch/=p;
	    unsigned8 mismatchdev=0;
	    for(int o=0;o<p; o++)
	      mismatchdev+=abs((signed8)mismatch-(signed8)offset_mismatch[o]);
	    mismatchdev/=p;
	    mismatch_array[p]=mismatch;
	  }

  double array_exp[block_count];
  for(int i = 0 ; i < block_count; i++)
    array_exp[i]=0;
  
#ifdef FIT_CURVES
  /**
   * Determine curve fitting parameters alfa and beta based on the following
   * formula: beta*(log(p+base^alfa)-alfa)
   * this formula will go through the origin and should be as close as possible 
   * to all other points. The beta parameter can be deteremined based on the alfa
   * choice, otherwise it acts as a search algorithm between two boundaries
   * 
   * Now, because we want to scim the tops we will always retain and use the last maximum value
   * so this is not a true average as one would expect it. This is a nice solution.
   */
  double average=0;
  double maxsofar=0;
  for(int p = 0 ; p < pscan_stop; p++)
    {
      double v=mismatch_array[p];
      if (v>maxsofar) maxsofar=v;
      average+=maxsofar;
    }
  average/=pscan_stop;
  
  int n=pscan_stop-1;
  double best_fit_alfa=0;
  double best_fit_beta=0;
  double best_fit=n*10000;
  for(double alfa=-20;alfa<20;alfa+=1)
    {
      /**
       * We calculate beta in function of the target average
       */
      double beta=average*n/(-alfa*n+n*log(exp(alfa)+n)-n);
      /**
       * So now we can see how wel this thing fits
       */
      double fitsofar=0;
      for(int p = 0 ; p < pscan_stop; p++)
	{
	  double val1=(log(exp(alfa)+p)-alfa)*beta;
	  double val2=mismatch_array[p];
	  fitsofar+=fabs(val2-val1);
	}
      if (fitsofar<best_fit)
	{
	  best_fit=fitsofar;
	  best_fit_alfa=alfa;
	  best_fit_beta=beta;
	}
    }
  // and who would care about that ?
  //  printf("alfa=%g; beta=%g\n",best_fit_alfa,best_fit_beta);

  /**
   * Reposition the mismatch_array data in function of the log
   */
  for(int p = 1; p<pscan_stop; p++)
    {
      double v1=mismatch_array[p];
      double v2=(log(exp(best_fit_alfa)+p)-best_fit_alfa)*best_fit_beta;
      double v=v1-v2;
      array_exp[p]=v2;
      mismatch_array_dbl[p]=v;
    }
#endif
 
  /**
   * Here we normalize the mismatcharry to have a maximum value of 0
   * and all other values below zero
   */
  double mx=mismatch_array_dbl[pscan_start];
  for(int p = pscan_start; p<pscan_stop; p++)
    {
      double v=mismatch_array_dbl[p];
      if (v>mx) mx=v;
    }
  for(int p = pscan_start; p<pscan_stop; p++)
    mismatch_array_dbl[p]-=mx;
  
  /**
   * Find the minimum 
   */
  int minerrat=pscan_start;
  double minerr=mismatch_array_dbl[pscan_start]*mismatch_array_freq[pscan_start];
  for(int p = pscan_start; p<pscan_stop; p++)
    {
      double v1=mismatch_array_dbl[p];
      double v2=mismatch_array_freq[p];
      double v=v1*v2;
      if (v<minerr)
	{
	  minerrat=p;
	  minerr=v;
	}
    }
  
  // the calculation of period to tempo is conducted as follows
  tempo=minerrat*stride/4;
  tempo=(samplerate/tempo)*60.0;
  status("Tempo is %g",tempo);

  if (plot)
    {
      Axis* hor=horizontal();
      axes(2,hor,axis("relative positioning"),"Reversed Autodifference");
      axes(3,hor,axis("relative positioning"),"Energy Envelope Spectrum");
      axes(4,hor,axis("relative positioning"),"Both combined");
      
      for(int p = 1; p<pscan_stop; p++)
	{
	  // the calculation of period to tempo is conducted as follows
	  double tempo=p*stride   // samples for 1 measure
	    /4;                   // samples per beat
	  tempo=(samplerate/tempo)  // beats per second
	    *60.0;                // beats per minute
	  double v4=-mismatch_array_dbl[p];
	  double v5=array_exp[p];
	  double v6=mismatch_array_freq[p];
	  period_point(2,p*stride,v4);
	  period_point(3,p*stride,v6);
	  period_point(4,p*stride,v6*v4);
	  /** 
	   * Columns
	   * 1 - tempo
	   * 2 - period
	   * 3 - rayshoot mismatch after normalization
	   * 4 - freq
	   * 5 - normalisation for rayshootmismatch
	   */
	}
    }
  finished();
  return false;  // don't continue, we did everything in one big chunk.
}
#endif // __loaded__bpm_analyzer_2010_cpp__
