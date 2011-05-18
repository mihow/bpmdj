/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__bpm_analyzer_cpp__
#define __loaded__bpm_analyzer_cpp__
using namespace std;
#line 1 "bpm-analyzer.c++"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <qspinbox.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <sys/mman.h>
#include <syscall.h>
#include <math.h>
#include <fftw3.h>
#include "bpm-analyzer.h"
#include "sys/times.h"
#include "version.h"
#include "player-core.h"
#include "scripts.h"
#include "bpm.h"
#include "memory.h"
#include "clock-drivers.h"

#define IMAGE_XS 320
#define IMAGE_YS 240
const signed4 shifter = 12;
const signed4 spectrum_shifter = 2;

/**
 * The smootmelody define will look at the melodyline and 'fix'it. Didn't work too well.
 */
//#define SMOOTH_MELODY

/**
 * FIT_CURVES will set the analyzer such that it uses a log function to normalize the resulting mismatchgraph
 * This feature improves the output of the algortihm from 30% to 40% successrate on a particular dataset.
 */
#define FIT_CURVES 

/**
 * The expected outcome modifies the output such that it will provide an estimate base don what 
 * was already seen. Comparing the measurement against what we expect it to be mioght hint which
 * tempos are interesting.
 */
//#define EXPECTED_OUTCOME

/**
 * FFT MIN_SINK will look atht the melody spectrum and lower each value based on the last minimum value.
 * RS_MAX_RAISE Perofrms the same stuff but  will instead raise the last element to its last known position.
 */
#define FFT_MIN_SINK
//#define RS_MAX_RAISE

/**
 * To deal with multiple levels of rayshooting we can define the following
 */
//#define MULTILEVEL

/**
 * To help me overload gnuplot, I export all note and frequency data into a nice large file
 */
//#define OUTPUT_BIGFILE

void map_output(float4* target, int startfreq, int count, double* hcfreq, int winsize)
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
      int bina=freqa*winsize/diskrate;
      int binb=freqb*winsize/diskrate;
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
void spectrum_to_bark(fftw_complex* in, int window_size, double* out)
{
  for(int b = 0 ; b < barksize ; b ++)
    {
      int a = (int)barkbounds[b];
      int c = (int)barkbounds[b+1];
      a *= window_size;
      c *= window_size;
      a /= diskrate;
      c /= diskrate;
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

/**
 * Deze functie leest niet alleen de audio maar normalizeert die ook
 * gebasseeerd op een windowsize van 1/16e beat.
 */
void read_audio(FILE* f,unsigned8 offset, double* target, unsigned4 length)
{
  fseek(f,offset*4,SEEK_SET);
  stereo_sample2 intaudio[length];
  unsigned4 pos=0;
  while(pos<length)
    {
      int read=fread(intaudio+pos,4,length-pos,f);
      assert(read>=0);
      pos+=read;
    }

  /**
   * The buffer contains the values that are currently added into the
   * sum.
   */
  unsigned4 windowsize=length/64;
  double buffer[windowsize];
  double absum=0;
  int j=0;
  for(unsigned4 i = 0 ; i < windowsize; i++)
    {
      double v=abs(intaudio[i].summed());
      absum+=v;
      buffer[i]=v;
    }
  double maxsum=0;
  for(unsigned4 i = 0 ; i < length; i++)
    {
      target[i]=absum;
      if (absum>maxsum) maxsum=absum;
      absum-=buffer[j];
      int k=(i+windowsize)%length;
      double v=abs(intaudio[k].summed());
      absum+=v;
      buffer[j]=v;
      j=(j+1)%windowsize;
    }
  if (maxsum==0) maxsum=1;
  for(unsigned4 i = 0 ; i < length; i++)
    target[i]/=maxsum;
}

// so the crosscorrelation has a bit of a drwaback namely that it
// prefers a 1/4th shift in most cases ? Why is that ?
// if the tempo is not measured completely accurately that might be a very natural response. 
void BpmAnalyzerDialog::experimental_scan()
{
  FILE * f,* g;
  /** 
   * load the full data into memory, barring the stereo information
   * that is not so very interesting to consider
   * We will work with 6 different fourier analyses, at lengts
   * 1024, 2048, 4096, 8192, 16384 and 32768. This means that we will need to read
   */
  const int largest_window=32768;
  const int stride=1024;
  FILE * raw = openCoreRawFile();
  assert(raw);
  audiosize=fsize(raw)/4;
  if (audiosize<largest_window) 
    {
      status("Not enough data to work with");
      return;
    }
  int block_count=1+(audiosize-largest_window)/stride;
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
      int window_size=window_sizes[widx];
      stereo_sample2 intaudio[window_size];
      double* han=bpmdj_allocate(window_size,double);
      /**
       * This is bloody wrong ! It should have an extra 
       * *2 in the x calculation. See below
       */
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
	  if (block%10==0)
	    updateReadingProgress(((double)widx+(double)block/(double)block_count)*100.0/6.0);
          // read this audio block
	  unsigned8 start=block;
	  start*=stride;
	  assert(start+window_size<=(unsigned8)audiosize);
	  fseek(raw,4*start,SEEK_SET);
	  int rd=readsamples(intaudio,window_size,raw);
	  assert(rd==window_size);
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
  
#ifdef SMOOTH_MELODY
  /**
   * In this step we also calculate the average of a number of frames so that we
   * can better pick out melody lines. An option here is to calculate the absdev
   * as well so that we can pick up on changes
   */
  for(int channel=0; channel<channel_count; channel++)
    {
      const int target_window_length_ms=1000;
      int samples_for_ms=target_window_length_ms*diskrate/1000;
      int block_window_size=samples_for_ms/stride;
      int n=block_count;
      float* c=bpmdj_allocate(n,float);
      // get data
      for(int x = 0 ; x < n; x++)
	c[x]=blocks[x][channel];
      // calculate sliding window average
      float* d=bpmdj_allocate(n,float);
      for(int x = 0 ; x < n; x++)
	{
	  int x1=x-block_window_size/2;
	  int x2=x1+block_window_size;
	  if (x1<0) x1=0;
	  if (x2>n) x2=n;
	  double avg=0;
	  for(int i=x1; i<x2; i++)
	    avg+=c[i];
	  d[x]=avg/(x2-x1);
	}
      // absdev
      float* e=bpmdj_allocate(n,float);
      for(int x = 0 ; x < n; x++)
	{
	  int x1=x-block_window_size/2;
	  int x2=x1+block_window_size;
	  if (x1<0) x1=0;
	  if (x2>n) x2=n;
	  double avg=0;
	  for(int i=x1; i<x2; i++)
	    avg+=fabs(c[i]-d[i]);
	  e[x]=avg/(x2-x1);
	  }
      // copy back
      for(int x = 0 ; x < n ; x++)
	blocks[x][channel]=d[x]+2*e[x];
      bpmdj_deallocate(c);
      bpmdj_deallocate(d);
      bpmdj_deallocate(e);
    }
    /**
    * This attempt: at larger windowsizes (1000ms) we see
    * a distinct effect on the selected tempos, not something we
    * want. So we muist use smaller window sizes (200ms or so).
    * The edge detection or not does not have much effect on the
    * frequency analysis, which is maybe what we want.
    * The effect on the rayshoot plot is yet to be determined.
    */
#endif
    
  /**
   * We scale each channel to fall in the 0..255 
   * range. Because we also want to make sure that the values are comparable accross channels
   * will we only normalize towards the total maximum and not per channel.
   */
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
  printf("Maximum and minimum are %g and %g\n",max,min);
  max-=min;
  if (max==0) max=1;
  for(int i = 0 ; i < block_count; i++)
    {
      float* channel=blocks[i];
      unsigned1* target=intblocks[i];
      for(int c = 0 ; c < channel_count; c++)
	target[c]=(unsigned1)(255.0*(channel[c]-min)/max);
    }
  updateReadingProgress(100);
  
  /**
   * Calculate the frequence content of each channel and store it back into
   * the blocks array.
   */
  double* notes=(fft_type*)fftw_malloc(block_count*sizeof(double));
  assert(notes);
  fftw_complex* hcfreq=(fftw_complex*)fftw_malloc(block_count*sizeof(fftw_complex));
  assert(hcfreq);
  fftw_plan plan = fftw_plan_dft_r2c_1d(block_count,notes,hcfreq,FFTW_MEASURE);
  assert(plan);
  for(int c = 0 ; c < channel_count; c++)
    {
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
  double bpmperbin=60.0*diskrate/(stride*block_count);
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
	  fprintf(g,"%g",60.0*i*diskrate/(stride*block_count));
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
  status("Rayshooting");
  
  /**
   * Now we start the rayshooting. Since we already have a relative large blocksize,
   * we can simply scan everyhing I believe
   * We could also print out the tempo for each of the different channels and
   * see which ones should carry more weight.
   */
  unsigned8 mismatch_array_min[block_count];
  unsigned8 mismatch_array_low[block_count];
  unsigned8 mismatch_array[block_count];
  double mismatch_array_dbl[block_count];
  unsigned8 mismatch_array_max[block_count];
  for(int i = 0 ; i < block_count; i++)
    {
      mismatch_array_min[i]=0;
      mismatch_array[i]=0;
      mismatch_array_dbl[i]=0;
      mismatch_array_low[i]=0;
      mismatch_array_max[i]=0;
    }

  /**
   * The start_bpm and stopbpm should be converted to the best matching period
   * which in this case is in blocks of stride size
   */
  int pscan_stop=((diskrate*60.0/startbpm) // samplecount per beat
		  *4                      // samples per bar
		  /stride);               // blocks per bar
  int pscan_start=((diskrate*60.0/stopbpm) // samplecount per beat
		   *4                     // samples per bar
		   /stride);              // blocks per bar
  if(pscan_start>2) pscan_start--;        // ensure we certainly cover the stopbpm;
  pscan_stop++;                           // ensure we conver startpbpm

  /**
   * Todo: print warning if bounds are weird
   */
  if (pscan_stop>=block_count/2) pscan_stop=(block_count/2);
  //  pscan_start=1;

  /**
   * Count the number of rays we will be seeing
   */
  unsigned4 raycount=0;
  for(int p=pscan_start;p<pscan_stop;p++)
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
#ifdef MULTILEVEL
  assert(pscan_stop*2<block_count);
  for(int p=1;p<pscan_stop*2;p++)
    {
      /**
       * Calculate the average mismatch for each offset
       */
      unsigned8 mismatch=0;
      unsigned8 additions=0;
      int i=block_count-p;
      assert(i<block_count);
      while(--i>=0)
	{
	  unsigned1* I=intblocks[i];
	  unsigned1* J=intblocks[i+p];
	  int C=channel_count;
	  while(--C>=0)
	    mismatch+=abs((int)I[C]-(int)J[C]);
	  additions++;
	}
      mismatch/=additions;
      mismatch_array[p]=mismatch;
    }
  /**
   * Calculate the mismatcharray on the mismatch_Arry itself and place it in
   * mismatch_array_min 
   */
  for(int p = 1 ; p < pscan_stop; p++)
    {
      unsigned8 mismatch=0;
      unsigned8 additions=0;
      int i=pscan_stop*2-p;
      while(--i>=0)
	{
	  mismatch+=abs((signed8)mismatch_array[i]-(signed8)mismatch_array[i+p]);
	  additions++;
	}
      mismatch_array_min[p]=mismatch/additions;
    }
  /**
   * The advanatage of this mismatcharray iteration is that we 
   * can fairly confident say that the tempo will be a multiplier of the
   * period found in the resulting array.
   * Which period we need to choose is an entirely different question of 
   * course. So if we need to choose between around 16 different
   * periods that all might be the underlying tempo
   * Once we have the essential idea of the tempo we should be able to 
   * extract all these entries out of the music and sort them in some form
   * of sensible array. How many beats do we have in this case ?
   * If we on average have around 1435 beats in a song then we should be able to extract
   * those single entitites out of it. Well, we could also work with the 18 samples we
   * work with. That would mean that we end up with 
   */
  
#else // -----multilevel finished-----
#ifdef PERNOTE
  f=fopen("tempo_at_note.txt","wb"); 
  for(int note=0; note < channel_count; note++)
    {
#endif

#if defined(FIT_CURVES) || defined(RS_MAX_RAISE) || defined(EXPECTED_OUTCOME)
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
#ifdef PERNOTE
		      int C=note;
#else
		      int C=channel_count;
		      while(--C>=0)
#endif
			mismatch+=abs((int)I[C]-(int)J[C]);
		      additions++;
		      raycount++;
		      i-=p;
		    }
		  updateProcessingProgress(raycount*100/total_rays);
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
	      mismatch_array_min[p]=mismatch_min;
	      mismatch_array[p]=mismatch;
	      mismatch_array_low[p]=mismatch-mismatchdev;
	      mismatch_array_max[p]=mismatch+mismatchdev;
#ifdef ABSDEV
	      /**
	       * Calculate the average distance to the average mismatch
	       */
	      unsigned8 absdev=0;
	      for(int o=0; o < p; o++)
		{
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
#ifdef PERNOTE
		      int C=note;
#else
		      int C=channel_count;
		      while(--C>=0)
#endif
			int diff=abs((int)I[C]-(int)J[C]);
		      absdev+=abs((signed int)mismatch-diff);
		      i-=p;
		    }
		}
	      //	  absdev*=2;
	      absdev/=additions;
	      if (absdev==0) absdev=1;
	      /**
	       * now we go again over all the data and figure out those that are sufficiently
	       * different to matter.
	       */
	      mismatch=0;
	      for(int o=0; o < p; o++)
		{
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
#ifdef PERNOTE
		      int C=note;
#else
		      int C=channel_count;
		      while(--C>=0)
#endif
			int diff=abs((int)I[C]-(int)J[C]);
		      if (diff>absdev) mismatch+=diff*100/absdev;

		      // so here we have an interesting
		      // problemn. if we ++ then we get low values
		      // if we += then we get high tempos, why is it that
		      // the valuestrenght itself affects the outcome so strongly ??
		      // maybe we hsould merely divide by the absolute deviation

		      /**
		       * I would be quite interested to generate two dataset
		       * one that will generate a slanted curve and one that
		       * will generate a flat curve
		       * If we have random data then small tempos will add a number
		       * of random elements
		       */
		      i-=p;
		    }
		}
	      /**
	       * Scale it to a sensible range
	       */
	      mismatch*=10;
	      mismatch/=additions;
	      mismatch_array[p]=mismatch;
#endif //ABSDEV
	    }
#ifdef PERNOTE
      /**
       * find the minimum error
       */
      int minerrat=pscan_start;
      unsigned int minerr=mismatch_array[pscan_start];
      for(int p = pscan_start+1; p<pscan_stop; p++)
	{
	  unsigned8 v=mismatch_array[p];
	  if (v<minerr)
	    {
	      minerrat=p;
	      minerr=v;
	    }
	}
      // the calculation of period to tempo is conducted as follows
      double tempo=minerrat*stride/4;
      tempo=(diskrate/tempo)*60.0;
      fprintf(f,"%d\t%g\n",note,tempo);
      printf("%d\t%g\n",note,tempo);
    }
#endif

#endif // non MULTILEVEL Branch
  /**
   * There is something wrong here.
   * structured music -> flatter profile
   * music with jitter and noise -> slanted profile
   * Neither is really acceptable, we don't want to have a difference based 
   * on solely this. Consequently we might need to calculate the 
   * standard deviation and based on that decide whether the change is worth the trouble or whether 
   * the change is not worth that much. This of course implies that we
   * go the long way of calculating standard defviations after valavulating means for
   * each specific ray. A messy process. It is also clear that each channel has its own 
   * noise fingerprint
   */

#ifndef PERNOTE

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
  printf("alfa=%g; beta=%g\n",best_fit_alfa,best_fit_beta);

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

#ifdef EXPECTED_OUTCOME
  /**
   * Calculate the assumption for future values
   */
  double array_cnt[block_count];
  for(int i = 0 ; i < block_count; i++)
    array_cnt[i]=0;
  for(int p = 1; p < pscan_stop; p++)
    for(int q=p+1; q<pscan_stop; q++)
      {
	int o=(q-p)%(p+1);
	double v=mismatch_array[o];
	if (v!=0)
	  {
	    array_exp[q]+=v;
	    array_cnt[q]++;
	  }
      }
  for(int i = 0 ; i < block_count; i++)
    {
      double cnt=array_cnt[i];
      if (cnt) array_exp[i]/=cnt;
    }  
#endif

#ifdef RS_MAX_RAISE
  /**
   * Now, because we want to scim the tops we will always retain and use the last maximum value
   * so this is not a true average as one would expect it. This is a nice solution.
   */
  double maxsofar=0;
  for(int p = 0 ; p < pscan_stop; p++)
    {
      double v=mismatch_array[p];
      if (v>maxsofar) 
	maxsofar=v;
      mismatch_array_dbl[p]=v-maxsofar;
      array_exp[p]=maxsofar;
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
  printf("Maximum is %g\n",mx);
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
  double tempo=minerrat*stride/4;
  tempo=(diskrate/tempo)*60.0;
  printf("Tempo is %g\n",tempo);
  set_measured_period_diskrate("Melodic Rayshooting",minerrat*stride);
  
  f=fopen("mismatch.txt","wb");
  for(int p = 1; p<pscan_stop; p++)
    {
      // the calculation of period to tempo is conducted as follows
      double tempo=p*stride  // samples for 1 measure
	/4;                  // samples per beat
      tempo=(diskrate/tempo)  // beats per second
	*60.0;               // beats per minute
      unsigned8 v1=0;
#ifdef MULTILEVEL
      v1=mismatch_array_min[p];
#endif
      /*
      unsigned8 v2=mismatch_array_low[p];
      unsigned8 v3=mismatch_array_max[p];
      */
      double v4=mismatch_array_dbl[p];
      double v5=array_exp[p];
      double v6=mismatch_array_freq[p];
      /** 
       * Columns
       * 1 - tempo
       * 2 - period
       * 3 - rayshoot mismatch
       * 4 - normalisation for rayshootmismatch
       * 5 - freq
       * 6 - rayshoot 2nd batch
       */
      fprintf(f,"%g\t%d\t%g\t%g\t%g\t%lld\n",tempo,p,v4,v5,v6,v1);
    }
#endif // PERNOTE ?
    fclose(f);

    /**
     * Advantage of rayshooting:
     * first the rayshooter is sensitive to noise in a parituclar channel.
     * If there is a lot of noise, the mismatch in that channel goes up.
     * a good thing.
     *
     * A foruier transform is less sensitive to particular areas.
     * perform for the same windowsize a desnsitiy analysis which frequencies
     * are actually measured.
     */
#ifdef ATTEMPTED_REFINEMENT
    /**
     * In this step we start to correlate the measures between each other thereby allowing a 10% shift from 
     * one measure to the next. IF a measure contains 4 beats, then a 10% shift in either direction is a 0.4 shift
     * of a beat, which should suffice to cover for annoying guitarists that play without metronome
     * 
     * The calculation repositions the start of each measure.
     */
    unsigned8 measure_period=minerrat*stride;
    int measure_starts=audiosize/measure_period;
    unsigned8* measure_start=bpmdj_allocate(measure_starts,unsigned8);
    for(int i = 0 ; i < measure_starts; i++)
      measure_start[i]=i*measure_period;
    
    fft_type* measure_odd=(fft_type*)fftw_malloc(measure_period*sizeof(double));
    fft_type* measure_even=(fft_type*)fftw_malloc(measure_period*sizeof(double));
    fft_type* crosscor=(fft_type*)fftw_malloc(measure_period*sizeof(double));
    fftw_complex *measure_odd_f=(fftw_complex*)fftw_malloc(measure_period*sizeof(fftw_complex));
    fftw_complex *measure_even_f=(fftw_complex*)fftw_malloc(measure_period*sizeof(fftw_complex));
    fftw_complex *crosscor_f=(fftw_complex*)fftw_malloc(measure_period*sizeof(fftw_complex));
    fftw_plan plan_odd = fftw_plan_dft_r2c_1d(measure_period,measure_odd,measure_odd_f,FFTW_ESTIMATE);
    fftw_plan plan_even = fftw_plan_dft_r2c_1d(measure_period,measure_even,measure_even_f,FFTW_ESTIMATE); 
    fftw_plan plan_crosscor = fftw_plan_dft_c2r_1d(measure_period,crosscor_f,crosscor,FFTW_ESTIMATE); 
    
    printf("Measure period = %lld\n",measure_period);
    read_audio(raw,0,measure_even,measure_period);
    fftw_execute(plan_even);
    f=fopen("crosscors.txt","wb");
    unsigned4 scanrange=measure_period/10; // 10% drift allowed, which is still a lot
    for(int i = 1 ; i < measure_starts; i++)
      {
	// read in data and calculate frequencies
	fftw_complex* first;
	fftw_complex* second;
	if (i%2)
	  {
	    read_audio(raw,measure_start[i],measure_odd,measure_period);
	    fftw_execute(plan_odd);
	    first=measure_even_f;
	    second=measure_odd_f;
	  }
	else
	  {
	    read_audio(raw,measure_start[i],measure_even,measure_period);
	    fftw_execute(plan_even);
	    first=measure_odd_f;
	    second=measure_even_f;
	  }
	// calculate cross correlation
	for(unsigned4 j = 0 ; j < measure_period; j++)
	  {
	    double a=first[j][0];
	    double b=first[j][1];
	    double c=second[j][0];
	    double d=second[j][1]; 
	    // (a+bi)(c-di)=a*c+bci-adi+bd=ac+bd + (bc-ad)i
	    double e=a*c+b*d;
	    double f=b*c-a*d;
	    assert(e>=0);
	    assert(f>=0);
	    crosscor_f[j][0]=e;
	    crosscor_f[j][1]=f;
	  }
	fftw_execute(plan_crosscor);
	
	// find maximum cross correlation 
	double crosscor_max=0;
	int crosscor_maxat=0;
	char fn[100];
	sprintf(fn,"crosscors-%d.txt",i);
	g=fopen(fn,"wb");
	assert(g);
	for(unsigned4 j = measure_period-scanrange ; j < measure_period+scanrange; j++)
	  {
	    int k=j%measure_period;
	    double v=crosscor[k];
	    if (v>crosscor_max) 
	      {
		crosscor_max=v;
		crosscor_maxat=(int)j-(int)measure_period;
	      }
	    fprintf(g,"%d\t%g\n",(int)j-(int)measure_period,v);
	  }
	fclose(g);
	fprintf(f,"%d\t%d\n",i,crosscor_maxat);
      }
    fclose(f);
    // so the interesting problem now is that the crosscorrelation as described here has the problem that it 
#endif
    
    /**
     * In the following bisective loop we will reposition the central point each time
     * so that the final mismatch becomes as small as possible. We still rely on the blocks we dealt
     * with before. This is something we learned from the ATTEMPTED_REFINMENT
     * The bisecxtion is done by having a remapped coordinate system that is bisected recursively
     * each of the 'remapped coordinates' has a 'comes_from' coordinate which is modified 
     * to provide the best overlay. The recursive process is iterated until the distance between
     * two measurement points falls below two measures.
     *
     * Important to note here is that the measure_period is used on the mapped coordinates since 
     * these will becomes the normal coordinates and we do not wa<nt to modify the tempo, we mainly want to bring
     * the originating coordinates in line.
     * 
     * In advance we calculate how many elements we will need. This will always be a power of two.
     */
    unsigned4 measure_period=minerrat;
    g=fopen("beatline.txt","wb");
    // printf("Measure period is %d\n",measure_period);

    unsigned8 range=block_count;
    unsigned1 powers=0;
    while(range>measure_period*2)
      {
	powers++;
	range/=2;
      }
    unsigned8 coors=1<<powers;
    unsigned8 mapped[coors+1];
    unsigned8 comesfrom[coors+1];
    mapped[0]=0;
    mapped[coors]=block_count;
    comesfrom[0]=0;
    comesfrom[coors]=block_count;
    for(unsigned1 power=0; power< powers; power++)
      {
	int stride=coors>>power;
	assert(stride>1);
	printf("Level %d\n",power);
	for(int range=stride; range<=coors; range+=stride)
	  {
	    unsigned8 remapped_1=mapped[range-stride];
	    unsigned8 remapped_2=mapped[range];
	    printf("Find between %lld and %lld\n",remapped_1,remapped_2);
	    unsigned8 comes_from_1=comesfrom[range-stride];
	    unsigned8 comes_from_2=comesfrom[range];
	    
	    char tmp[100];
	    sprintf(tmp,"beatline-%d-%d.txt",power,range/stride);
	    f=fopen(tmp,"wb");
	    assert(f);
	    
	    assert(remapped_2>remapped_1);
	    assert(comes_from_2>comes_from_1);  
	    unsigned8 remapped_center=(remapped_1+remapped_2)/2;
	    mapped[range-stride/2]=remapped_center;

	    /**
	     * This is all a bit tricky on the variables used and the boundaries
	     * used. In the end we will have a slant that will be refined, meaning
	     * that we will have two boundaries that actually map to a different 
	     * position, it is in this refined position that we traverse
	     * to the target and we must figure out from where the data is actually
	     * taken.
	     * 
	     * The second problem is that not all rays will fall within the specified 
	     * range, in which case we should omit 
	     *
	     * Third: the shift always repositions the middle point of the remapped
	     * coordinates so that we can safely
	     */
	    signed8 comes_from_old_center=(comes_from_1+comes_from_2)/2;
	    signed8 comes_from_range=(comes_from_2-comes_from_1)/16;
	    minerrat=-1;
	    for(signed8 comes_from_center=comes_from_old_center-comes_from_range; 
		comes_from_center<comes_from_old_center+comes_from_range; comes_from_center++)
	      {
		unsigned8 mismatch=0;
		for(unsigned4 o = 0 ; o < measure_period ; o++)
		  {
		    unsigned1* I=NULL;
		    unsigned1* J=NULL;
		    for(signed4 remapped = remapped_1+o ; remapped < remapped_2; remapped+=measure_period)
		      {
			signed4 comes_from;
			if (remapped<remapped_center)
			  comes_from=comes_from_1+
			    (remapped-remapped_1)
			    *(comes_from_center-comes_from_1)
			    /(remapped_center-remapped_1);
			else
			  comes_from=comes_from_center+
			    (remapped-remapped_center)
			    *(comes_from_2-comes_from_center)
			    /(remapped_2-remapped_center);
			assert(comes_from>=0);
			if(comes_from>=block_count) break;
			J=intblocks[comes_from];
			if (I) 
			  {
			    int C=channel_count;
			    while(--C>=0)
			      mismatch+=abs((int)I[C]-(int)J[C]);
			  }
			I=J;
		      }
		  }
		if (minerrat==-1 || mismatch<minerr) 
		  {
		    minerr=mismatch;
		    minerrat=comes_from_center;
		  }
		fprintf(f,"%lld\t%lld\n",comes_from_center-comes_from_old_center,mismatch);
	      }
	    fclose(f);
	    comesfrom[range-stride/2]=minerrat;
	    fprintf(g,"%lld\t%d\n",remapped_center,minerrat);
	  }
      }
    fclose(g);

    g=fopen("beatline-hires.txt","wb");
    assert(g);
    stereo_sample2* source=(stereo_sample2*)mmap(NULL,audiosize*4,PROT_READ,MAP_PRIVATE,fileno(raw),0);
    assert(source);
    if (source==MAP_FAILED)
      printf("Source mapping failed: %s\n",strerror(errno));
    int targetfd=::open("mappedaudio.raw",O_RDWR|O_CREAT,S_IRWXU);
    int mappedfd=::open("mappeddoubles.raw",O_RDWR|O_CREAT,S_IRWXU);
    assert(targetfd>0);
    assert(mappedfd>0);
    ftruncate(targetfd,audiosize*4);
    ftruncate(mappedfd,audiosize*sizeof(double));
    stereo_sample2* target=(stereo_sample2*)mmap(NULL,audiosize*4,PROT_READ|PROT_WRITE,MAP_SHARED,targetfd,0);
    double* M=(double*)mmap(NULL,audiosize*sizeof(double),PROT_READ|PROT_WRITE,MAP_SHARED,mappedfd,0);
    assert(target);
    assert(M);
    if (target==MAP_FAILED)
      printf("Target mapping failed: %s\n",strerror(errno));
    if (M==MAP_FAILED)
      printf("Mapped mapping failed: %s\n",strerror(errno));
    // and now the mess of repositioning all samples in the audio file so that we can plot the
    // outcome.
    printf("Creating new output file\n");
    for(unsigned8 i = 0 ; i< coors; i++)
      {
	unsigned8 to_1=mapped[i]*stride;
	unsigned8 to_2=mapped[i+1]*stride;
	unsigned8 from_1=comesfrom[i]*stride;
	unsigned8 from_2=comesfrom[i+1]*stride;
	assert(to_1<audiosize);
	assert(to_2<audiosize);
	for(unsigned8 to=to_1; to<to_2; to++)
	  {
	    unsigned8 from=from_1 + (to-to_1)*(from_2-from_1)/(to_2-to_1);
	    assert(from<audiosize);
	    stereo_sample2 sample=source[from];
	    target[to]=sample;
	    M[to]=(double)sample.summed()/65536.0;
	    //if (to%100==0)
	    //	      fprintf(g,"%lld\t%lld\n",from,to);
	  }
      }
    fclose(g);
    munmap(source,audiosize*4);
    munmap(target,audiosize*4);
    ::close(targetfd);
    msync(M,audiosize*sizeof(double),MS_SYNC);

    /**
     * Now we generate a target overlay that consists of two measures
     * with lo, mid and hi content for each frequency band. 
     * Why would we want to do that anyway ? In the end we could as well
     * just summarize the notes into octaves instead of recalculating the
     * thing. We might believe the resoltion improves, which might be true
     * We could also believe it goes faster ? The barkscale might make 
     * slightly more sense for the appropriate alignment here ?
     * Or really the note based view ? The latter has an advantage that we can choose ourselves
     * how to deal with the further divisions.
     * Or we could simply have the full spectrum of 2048 elements. Welll.. that wouldn't work
     * since we still need to compare them across songs. So better is to have a predefined collection
     * of targetfrequencies. Well, with our potential 6% shift, we do modify the frequencies subtantially
     * so maybe we can better split the first number of octaves in 2 and then. The barkbounds do satisfy 
     * me oddly enough. So barkbounds it be.
     *
     * Each measure is split in 32th notes. To improve
     * the sensitivity to events we generate 8 different subalignments
     * and select the one with the highest RMS value in the first differential
     */
    unsigned8 dmeasure_samples=measure_period*stride*2;
    unsigned8 divisor=2*audiosize/measure_period;
    unsigned8 window_size=measure_period*stride/32;
    unsigned8 offset_stride=window_size/8;
    
    double results[8][spectrum_size][64];
    for(int i = 0 ; i < 8 ; i++)
      for(int j = 0 ; j < spectrum_size; j++)
	for(int k = 0 ; k < 64; k++)
	  results[i][j][k]=0;
    
    fft_type* in=(fft_type*)fftw_malloc(window_size*sizeof(fft_type)); assert(in);
    fftw_complex* out=(fftw_complex*)fftw_malloc(window_size*sizeof(fftw_complex)); assert(out);
    plan = fftw_plan_dft_r2c_1d(window_size,in,out,FFTW_MEASURE);
    assert(plan);
    
    double* han=bpmdj_allocate(window_size,double);
    for(int i = 0 ; i < window_size; i++)
      {
	double x=i*2.0*M_PI/(double)window_size;
	x-=M_PI/2;
	han[i]=(sin(x)+1)/2;
      }
    
    for(int offset=0; offset<8; offset++)
      {
	printf("Scanning at offset %d\n",offset);
	for(unsigned8 dmeasure_start=offset*offset_stride; 
	    dmeasure_start+dmeasure_samples<audiosize; 
	    dmeasure_start+=dmeasure_samples)
	  {
	    printf("  Double measure %d ",(int)(dmeasure_start/dmeasure_samples));
	    for(unsigned8 tick=0; tick < 64; tick++)
	      {
		if (tick%2) printf(".");
		unsigned8 from=dmeasure_start+tick*dmeasure_samples/64;
		for(int i = 0; i< window_size; i++)
		  {
		    assert(M[i+from]<=1.0);
		    assert(M[i+from]>=-1.0);
		    double v=M[i+from];
		    in[i]=v;
		  }
		fftw_execute(plan);
		
		double bark_presence[spectrum_size];
		spectrum_to_bark(out,window_size,bark_presence);
		
		/* 
		   for(int i = 0 ; i < window_size; i++)
		   {
		   double a=out[i][0];
		   double b=out[i][1];
		   double c=sqrt(a*a+b*b)/(window_size*divisor);
		   if (isnan(c)) c=0;
		   out[i][0]=c;
		   }
		// here we extract the frequency content and store it into the result array
		// band0 goes to 100 Hz, band 2 goes to 880 Hz, band 3 is evertyhing higher
		int hz100=window_size*100/diskrate;
		int hz880=window_size*880/diskrate;
		double lo=0,mid=0,hi=0;
		for(int j=0; j < hz100; j++)
		  lo+=out[j][0];
		lo/=hz100;
		for(int j=hz100; j < hz880; j++)
		  mid+=out[j][0];
		mid/=hz880-hz100;
		for(int j=hz880; j < window_size/2; j++)
		  hi+=out[j][0];
		hi/=window_size/2-hz880;
		// store the results
		results[offset][0][tick]+=lo;
		results[offset][1][tick]+=mid;
		results[offset][2][tick]+=hi;
		*/
		
		for(int b=0; b<spectrum_size; b++)
		  results[offset][b][tick]+=bark_presence[b]/divisor;
	      }
	    printf("\n");
	  }
      }
    
    /**
     * Here we normalize the outcome by selecting the output with the largest first differential.
     * the problem is of course that we should or should not maybe normalize the channel itself
     * before we write it to disc ? It might make more sense not to do so. We can always normalize 
     * later as well.
     */
    for(int offset=0; offset<8; offset++)
      {
	// find maximum
	double ma=results[offset][0][0];
	double mi=ma;
	for(int c=0; c < spectrum_size; c++)
	  for(int tick = 0; tick < 64; tick++)
	    {
	      double v=results[offset][c][tick];
	      if (v>ma) ma=v;
	      if (v<mi) mi=v;
	    }	
	ma-=mi;
	if (ma==0) ma=1;
	for(int c=0; c < spectrum_size; c++)
	  for(int tick = 0; tick < 64; tick++)
	    {
	      double v=results[offset][c][tick];
	      results[offset][c][tick]=(v-mi)/ma;
	    }
      }
	
    int bestoffset=0;
    double largestvar=0;
    for(int offset=0; offset<8; offset++)
      {
	double sd=0;
	for(int c = 0 ; c < spectrum_size; c++)
	  {
	    double p=results[offset][c][63];
	    for(int tick = 0; tick < 64; tick++)
	      {
		double l=results[offset][c][tick];
		sd+=fabs(l-p);
		p=l;
	      }
	  }
	if (bestoffset<0 || sd>largestvar)
	  {
	    largestvar=sd;
	    bestoffset=offset;
	  }
      }
    printf("Would prefer offset %d\n",bestoffset);

    /**
     * Now we need to write the crap to a file so that we can 
     * learn data from it. The targetfilename
     */
    char tmp[1024];
    sprintf(tmp,"%s.re",argument);
    f=fopen(tmp,"wb");
    assert(f);
    for(int c= 0; c<spectrum_size; c++)
      {
	int r;
	r=fwrite(results[bestoffset][c],64*sizeof(double),1,f);
	assert(r>=0);
      }
    fclose(f);

    /**
     * Dump lovely debugging info
     */
    g=fopen("rhythm.txt","wb");
    assert(g);
    for(int t=0;t<64;t++)
      {
	fprintf(g,"%d",t);
	for(int c=0;c<spectrum_size;c++)
	  fprintf(g,"\t%g",results[bestoffset][c][t]);
	fprintf(g,"\n");
      }
    
    // the end
    printf("Done\n");
    fclose(raw);
}

BpmAnalyzerDialog::BpmAnalyzerDialog(QWidget*parent) : QWidget(parent)
{
  setupUi(this);
  player=NULL;
  tapcount=0;
  audio=NULL;
  audiorate = 11025;
  startbpm = 120;
  stopbpm = 160;
  startshift = 0;
  stopshift = 0;
  bufsiz = 32 * 1024;
  freq = NULL;
  reading_progress = 0;
  processing_progress = 0;
  // clear the image
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
  BpmPix->setPixmap(*pm);
}

unsigned4 BpmAnalyzerDialog::phasefit(unsigned4 i)
{
  unsigned4 c;
  unsigned4 mismatch=0;
  for(c=i;c<(unsigned4)audiosize;c++)
    mismatch+=abs((signed4)audio[c]-(signed4)audio[c-i]);
  return mismatch;
}

unsigned4 BpmAnalyzerDialog::phasefit(unsigned4 i, unsigned4 clip)
{
  unsigned4 c;
  unsigned4 mismatch=0;
  if (!clip) return phasefit(i);
  assert(audiosize>=0 && i<(unsigned4)audiosize);
  for(c=i;c<(unsigned4)audiosize && mismatch <= clip ;c++)
    mismatch+=abs((signed4)audio[c]-(signed4)audio[c-i]);
  return mismatch;
}

void BpmAnalyzerDialog::setBpmBounds(signed4 start, signed4 stop)
{
  char tmp[500];
  char tmp2[500];
  startbpm=start;
  stopbpm=stop;
  sprintf(tmp,"%d",startbpm);
  FromBpmEdit->setText(tmp);
  sprintf(tmp2,"%d",stopbpm);
  ToBpmEdit->setText(tmp2);
}

void BpmAnalyzerDialog::updateReadingProgress(signed4 val)
{
  if (reading_progress!=val)
    {
      reading_progress=val;
      updateInfo();
    }
}


void BpmAnalyzerDialog::updateProcessingProgress(signed4 val)
{
  if (processing_progress!=val)
    {
      processing_progress = val;
      updateInfo();
    }
}

void BpmAnalyzerDialog::updateInfo()
{
  ReadingBar->setValue(reading_progress);
  CountingBar->setValue(processing_progress);
  app->processEvents();
}

void BpmAnalyzerDialog::readAudio()
{
  FILE * raw = openCoreRawFile();
  audiosize=fsize(raw);
  signed4 startpercent=0;
  startpercent-=startpercent%4;
  signed4 stoppercent=audiosize;
  stoppercent-=stoppercent%4;
  audiosize=stoppercent-startpercent;
  audiosize/=(4*(diskrate/audiorate));
  audio=bpmdj_allocate(audiosize+1,unsigned1);
  status("Reading");
  unsigned4 pos=0;
  signed4 count, redux, i;
  signed short buffer[bufsiz];
  fseek(raw,startpercent,SEEK_SET);
  assert(audiosize>=0);
  while(pos<(unsigned4)audiosize && !stop_signal)
    { 
      // als hem 1000 zegt hebben we dus eigenlijk 2000 samples binnen gekregen
      unsigned4 *trgt = (unsigned4*)(void*)buffer;
      count = readsamples(trgt,bufsiz/2,raw);
      updateReadingProgress(pos*100/audiosize);
      for (i = 0 ; i < count * 2 ; i += 2 * (diskrate/audiorate) )
	{
	  signed4 left, right,mean;
	  left=abs(buffer[i]);
	  right=abs(buffer[i+1]);
	  mean=(left+right)/2;
	  redux=abs(mean)/128;
	  if (pos+i/(2*(diskrate/audiorate))>=(unsigned4)audiosize) break;
	  assert(pos+i/(2*(diskrate/audiorate))<(unsigned4)audiosize);
	  audio[pos+i/(2*(diskrate/audiorate))]=(unsigned1)redux;
	}
      pos+=count/(diskrate/audiorate);
    }
  fclose(raw);
  if (stop_signal)
    {
      bpmdj_deallocate(audio);
      audio=NULL;
      audiosize=0;
      status("Canceled while reading");
      return;
    }
  reading_progress = 100;
  updateInfo();
}

void BpmAnalyzerDialog::status(QString text)
{
  StatusLabel->setText(text);
  updateInfo();
}

void BpmAnalyzerDialog::readAudioBlock(signed4 blocksize)
{
  FILE * raw = openCoreRawFile();
  audiosize=fsize(raw);  // uitgedrukt in bytes...
  signed4 startpercent=0;
  startpercent/=4;       // uitgedrukt in sample dus...
  signed4 stoppercent=audiosize;
  stoppercent/=4;        // uitgedrukt in samples
  audiosize = stoppercent-startpercent;  // uitgedrukt in samples
  audiosize /= blocksize;        // uitgedrukt in blokken
  audio = bpmdj_allocate(audiosize, unsigned1);
  updateProcessingProgress(0);
  status("Reading audio (block size "+QString::number(blocksize)+")");
  unsigned4 pos = 0;
  signed4 count;
  stereo_sample2 buffer[blocksize];
  fseek(raw,startpercent*4,SEEK_SET);
  while((signed8)pos<(signed8)audiosize && !stop_signal)
    {
      count = 0;
      do 
	{
	  count = readsamples(buffer+count,blocksize-count,raw);
	}
      while (count < blocksize );
      updateReadingProgress(pos*100/audiosize);
      unsigned4 total = 0;
      for (signed4 i = 0 ; i < blocksize ; i ++ )
	{
	  signed4 left, right;
	  left=abs(buffer[i].left);
	  right=abs(buffer[i].right);
	  total+=(left+right)/256;
	}
      audio[pos]=total/blocksize;
      pos++;
    }
  fclose(raw);
  
  if (stop_signal)
    {
      bpmdj_deallocate(audio);
      audio=NULL;
      audiosize=0;
      status("Canceled while reading");
      return;
    }  
  reading_progress = 100;
  updateInfo();
}

void BpmAnalyzerDialog::rangeCheck()
{
  unsigned4 val;
  bool changed = false;
  val = FromBpmEdit->text().toInt();
  if (startbpm != val)
    startbpm=val;
  val = ToBpmEdit->text().toInt();
  if (stopbpm != val)
    stopbpm=val;
  if (changed && audio)
    {
	bpmdj_deallocate(audio);
	audio=NULL;
	audiosize=0;
    }
}

fft_type index2autocortempo(signed4 i)
{
  assert(i);
  fft_type measure_period_in_ticks = i<<spectrum_shifter;
  fft_type measure_period_in_secs  = measure_period_in_ticks/(fft_type)diskrate;
  fft_type measure_frequency_in_hz = 1/measure_period_in_secs;
  fft_type measure_frequency_in_bpm = measure_frequency_in_hz*60.0;
  fft_type beat_frequency_in_bpm = measure_frequency_in_bpm*4.0;
  return beat_frequency_in_bpm;
}

void BpmAnalyzerDialog::autocorrelate_draw(QPainter &p, signed4 xs, signed4 ys,
signed4 shifter)
{
  if (!freq) return;
  p.setPen(Qt::gray);
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

  signed4 lastx = 0, lasty = 0, lastcount = 0;
  for(signed4 i = 1 ; i <windowsize/2; i ++)
    {
      fft_type bpm = index2autocortempo(i);      
      // calculate position
      signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
      signed4 y = (signed4)((float8)ys-(float8)ys*freq[i]);
      if (x<0 || x>=xs) continue;
      if (y>=ys) continue;
      if (y<0) y=0;
      
      if (x!=lastx && lastcount)
	{
	  p.drawLine(lastx,lasty/lastcount,lastx,ys);
	  lastx=x;
	  lasty=0;
	  lastcount=0;
	}
      lasty+=y;
      lastcount++;
    }
  
  for (signed4 i = 0 ; i < peaks ; i ++)
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
}

void BpmAnalyzerDialog::fft_draw(QPainter &p, signed4 xs, signed4 ys, 
signed4 shifter, float8 bpm_divisor)
{
  if (!freq) return;
  p.setPen(Qt::gray);
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
  signed4 lastx = 0, lasty = 0, lastcount = 0;
  for(signed4 i = 0 ; i <windowsize/2; i ++)
    {
      // calculate BPM
      float8 bpm = Index_to_frequency(windowsize,i);  // relative wrt diskrate
      bpm*=(float8)diskrate;                // in Hz tov non collapsed diskrate
      for(signed4 j = 0 ; j < shifter; j ++) bpm/=2.0;  // collapsed diskrate
      bpm*=60.0/bpm_divisor;                                      // in BPM.
      // calculate position
      signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
      signed4 y = (signed4)((float8)ys-(float8)ys*freq[i]);
      if (x<0 || x>=xs) continue;
      if (y>=ys) continue;
      if (y<0) y=0;
      
      if (x!=lastx)
	{
	  if (lastcount)
	    p.drawLine(lastx,lasty/lastcount,lastx,ys);
	  lastx=x;
	  lasty=0;
	  lastcount=0;
	}
      lasty+=y;
      lastcount++;
    }
  for (signed4 i = 0 ; i < peaks ; i ++)
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
}

void BpmAnalyzerDialog::fft()
{
  signed8 audiosize;
  FILE * raw = openCoreRawFile();
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // deel door 2^shifter to bring us within een range van 322.9 BPM
  audiosize>>=shifter;
  signed4 blocksize = 1 << shifter;
  stereo_sample2 *block = bpmdj_allocate(blocksize,stereo_sample2);
  float8 *audio = (float8*)fftw_malloc(audiosize*sizeof(float8));
  windowsize = lower_power_of_two(audiosize);
  freq = (fft_type*)fftw_malloc(windowsize*sizeof(fft_type));
  fftw_plan plan = fftw_plan_r2r_1d(windowsize,audio,freq,FFTW_R2HC,
				    FFTW_MEASURE);
  for(signed4 i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].left);
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("First FFT called\n");
  fftw_execute(plan);
  // rescale the entire thing
  float8 max = 0;
  signed4 halfwindow = windowsize/2;
  for(signed4 i = 0 ; i <halfwindow ; i ++)
    {
      freq[i]=10.0*log(fabs(freq[i]));
      if (freq[i]>max) max=freq[i];
    }
  for (signed4 i = 0 ; i < halfwindow ; i ++)
    freq[i]/=max;
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = bpmdj_allocate(peaks, fft_type);
  peak_energy = bpmdj_allocate(peaks, fft_type);
  fft_type *copy = bpmdj_allocate(halfwindow, fft_type);
  for(signed4 i = 0 ; i < halfwindow ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // bpm left and right...
  
  for(signed4 j = 0 ; j < peaks ; j ++)
    {
      float8 energy = 0;
      float8 at = 0;
      for(signed4 i = 0 ; i <halfwindow ; i ++)
	{
	  // obtain bpm
	  // uitgedruk relatief tov diskrate
	  float8 bpm = Index_to_frequency(windowsize,i);
	  // uitgedrukt in Hz tov non collapsed diskrate
	  bpm*=(float8)diskrate;                           
	  // uitgedrukt in collapsed diskrate
	  for(signed4 j = 0 ; j < shifter; j ++) bpm/=2.0;  
	  // uitgedrukt in BPM.
	  bpm*=60.0;                         
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
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      // clear neighbors
      for(signed4 i = 0 ; i <halfwindow ; i ++)
	{
	  // obtain BPM
	  // uitgedruk relatief tov diskrate
	  float8 bpm = Index_to_frequency(windowsize,i);         
	  // uitgedrukt in Hz tov non collapsed diskrate
	  bpm*=(float8)diskrate;            
	  // uitgedrukt in collapsed diskrate
	  for(signed4 j = 0 ; j < shifter; j ++) bpm/=2.0;
	  // uitgedrukt in BPM.
	  bpm*=60.0;                        
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  
  fftw_destroy_plan(plan);
  fftw_free(audio);
}

void BpmAnalyzerDialog::enveloppe_spectrum()
{
  signed8 audiosize;
  FILE * raw = openCoreRawFile();
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
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
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].left);
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("Audio has been read...\n");
  fftw_execute(plan);
  printf("FFT has been done\n");
  // rescale the entire thing
  fft_type max = 0;
  fft_type min = -1.0;
  for(signed4 i = 0 ; i <windowsize/2 ; i ++)
    {
      fft_type bpm = Index_to_frequency(windowsize,i); // tov diskrate
      bpm*=(fft_type)diskrate;      // in Hz tov non collapsed diskrate
      // uitgedrukt in collapsed diskrate
      for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0; 
      bpm*=60.0;                                      // uitgedrukt in BPM.
      if (bpm<startbpm) continue;
      if (bpm>stopbpm) break;
      freq[i]=10.0*log(fabs(freq[i]));
      // freq[i]=fabs(freq[i]);
      if (freq[i]>max) max=freq[i];
      if (freq[i]<min || min < 0.0) min=freq[i];
    }
  printf("maximum =%g, minimum = %g\n",max,min);
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
      for(signed4 i = 0 ; i <windowsize/2 ; i ++)
	{
	  // tov diskrate
	  fft_type bpm = Index_to_frequency(windowsize,i); 
	  // in Hz tov non collapsed diskrate
	  bpm*=(fft_type)diskrate; 
	  // uitgedrukt in collapsed diskrate
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
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      if (j == 0) 
	set_measured_period_diskrate("Envelope",(signed4)(4.0*11025.0*60.0*4.0/at));
      
      // clear neighbors
      for(signed4 i = 0 ; i <windowsize/2 ; i ++)
	{
	  // obtain BPM
	  // relatief tov diskrate
	  fft_type bpm = Index_to_frequency(windowsize,i);
	  bpm*=(float8)diskrate;             // in Hz tov non collapsed diskrate
	  // in collapsed diskrate
	  for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;    
	  bpm*=60.0;                                      // in BPM.
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  fftw_destroy_plan(plan);
  fftw_free(audio);
}

void BpmAnalyzerDialog::autocorrelate_spectrum()
{
  signed8 audiosize;
  FILE * raw = openCoreRawFile();
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // OK, nu moeten we dat delen door 2^spectrum_shifter
  audiosize>>=spectrum_shifter;
  windowsize = lower_power_of_two(audiosize);
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
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=block[j].left;
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("Audio has been read...\n");
  
  // 1. do a FFT of the entire sample
  fftw_execute(forward_plan);
  fftw_destroy_plan(forward_plan);
  fftw_free(audio);
  printf("Forward FFT has been done\n");

  // 2. modify freq[i]=norm(freq[i])^2;
  for(signed4 i = 0 ; i < windowsize; i ++)
    {
      freq_tmp[i][0] = freq_tmp[i][0] * freq_tmp[i][0]
	+ freq_tmp[i][1] * freq_tmp[i][1];
      freq_tmp[i][1]=0;
    }
  printf("Copy has been made\n");
  
  // 3. do an inverse Fourier transform of freq[i]
  fftw_execute(backward_plan);
  fftw_destroy_plan(backward_plan);
  fftw_free(freq_tmp);
  printf("Backward FFT has been done\n");
  
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
  
  printf("maximum =%g, minimum = %g\n",max,min);
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
	set_measured_period_diskrate("Autocorrelation",
				     (signed4)(4.0*11025.0*60.0*4.0/at));
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      
      // clear neighbors
      for(signed4 i = 1 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = index2autocortempo(i);
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm<at-range) break;
	}
    }
}

void BpmAnalyzerDialog::wec()
{
  FILE * file = openCoreRawFile();
  assert(file);
  int fd = fileno(file);
  unsigned4 map_length = fsize(file);
  stereo_sample2 * audio = (stereo_sample2*)mmap(NULL,map_length,PROT_READ,
						 MAP_SHARED,fd,0);
  assert(audio!=MAP_FAILED);
  assert(audio);
  BpmCounter bc(stderr,audio,map_length/4,diskrate,startbpm,stopbpm);
  tempo_type result(bc.measure());
  munmap(audio,map_length);
  set_measured_period_diskrate("Wec",tempo_to_period(result).period*4);
}

void BpmAnalyzerDialog::set_labels()
{
  char d[500];
  sprintf(d,"%2g",(float8)startbpm+3.0*(float8)(stopbpm-startbpm)/4.0);
  X3->setText(d);
  sprintf(d,"%2g",(float8)startbpm+(float8)(stopbpm-startbpm)/2.0);
  X2->setText(d);
  sprintf(d,"%2g",(float8)startbpm+(float8)(stopbpm-startbpm)/4.0);
  X1->setText(d);
}

void BpmAnalyzerDialog::analyze()
{
  rangeCheck();
  set_labels();
  if (stop_signal) return;
  if (enveloppeSpectrum->isChecked())
    {
      enveloppe_spectrum();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      fft_draw(p, IMAGE_XS, IMAGE_YS, spectrum_shifter,1.0);
      p.end();
      BpmPix->setPixmap(*pm);
      return;
    }
  if (fullAutoCorrelation->isChecked())
    {
      autocorrelate_spectrum();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      autocorrelate_draw(p, IMAGE_XS, IMAGE_YS, spectrum_shifter);
      p.end();
      BpmPix->setPixmap(*pm);
      return;
    }
  if (weightedEnvCor->isChecked()) 
    {
      wec();
      return;
    }
  if (experimentalAlg->isChecked())
    {
      experimental_scan();
      return;
    }
  if (stop_signal) return;
  if (ultraLongFFT->isChecked())
    {
      fft();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      fft_draw(p, IMAGE_XS, IMAGE_YS, shifter,1.0);
      p.end();
      BpmPix->setPixmap(*pm);
    }
  if (stop_signal) return;
  if (resamplingScan->isChecked())
    {
      rayshoot_scan();
      return;
    }
  else
    {
      if (!audio) readAudio();
      if (stop_signal) return;
      if (ultraLongFFT->isChecked()) 
	peak_scan();
    }
}

void BpmAnalyzerDialog::rayshoot_scan()
{
  const unsigned4 blockshifter_max = 8;  
  unsigned4 blockshifter = blockshifter_max;
  signed* mismatch_array[blockshifter_max+1];
  unsigned4 mean[blockshifter_max+1];
  unsigned4 minimum_at = 0;
  unsigned4 minima[blockshifter_max+1];
  for (unsigned4 i = 0 ; i <= blockshifter_max; i++) mean[i]=0;
  unsigned4 maxima[blockshifter_max+1];
  // depending on the audiorate we should stop at a certain block shift 
  // minimum...
  unsigned4 blockshifter_min; 
  if (audiorate == 44100) blockshifter_min = 0;
  else if (audiorate == 22050) blockshifter_min = 1;
  else if (audiorate == 11025) blockshifter_min = 2;
  else assert(0);
  stopshift = 44100 * 60 * 4 / startbpm ;
  startshift = 44100 * 60 * 4 / stopbpm ;
  for( unsigned4 i = 0; i <= blockshifter_max ; i ++)
    {
      mismatch_array[i] = bpmdj_allocate(stopshift-startshift, signed);
      for(unsigned4 j = 0 ; j < stopshift-startshift ; j ++)
	mismatch_array[i][j] = -1;
    }
  while(blockshifter>=blockshifter_min && ! stop_signal)
    {
      unsigned4 blocksize = 1 << blockshifter;
      // first read audio
      readAudioBlock(blocksize);
      // calculate all mismatches
      signed * mismatch = mismatch_array[blockshifter] - startshift;
      signed * prev_mismatch = NULL;
      unsigned4 prev_maximum = 0;
      if (blockshifter < blockshifter_max)
	{
	  prev_mismatch = mismatch_array[blockshifter+1] - startshift;
	  // the mismatch array goes always from 0 to stophift-startshift-1
	  // the prev_mismatch  hence goes from  startshift to stopshift - 1
	  prev_maximum = mean[blockshifter+1]; 
	}
      status("Scanning "+QString::number(blockshifter_max-blockshifter)
	     +"/" + QString::number(blockshifter_max-blockshifter_min));
      if (!prev_mismatch)
	for (unsigned4 i = startshift ; i < stopshift && ! stop_signal; i ++ )
	  {
	    unsigned4 phase = i >> blockshifter;
	    unsigned4 store = phase << blockshifter;
	    updateProcessingProgress((i - startshift) *100 / 
				     (stopshift - startshift));
	    if (store!=i) continue;
	    unsigned4 m = phasefit(phase);
	    mismatch[store]=m;
	  }
      else
	{
	  // first a dry run to see how many times we would execute the 
	  // phasefit;
	  signed4 phasefit_called = 0;
	  signed4 phasefit_total = 0;
	  for (unsigned4 i = startshift ; i < stopshift && ! stop_signal; i++ )
	    {
	      unsigned4 phase = i >> blockshifter;
	      unsigned4 store = phase << blockshifter;
	      if (store!=i) continue;
	      unsigned4 prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned4 next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (prev_store < startshift ) continue;
	      if (next_store >= stopshift ) break;
	      // sign is important !
	      unsigned4 prev_val = prev_mismatch[prev_store]; 
	      // sign is important !
	      unsigned4 next_val = prev_mismatch[next_store]; 
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		phasefit_total++;
	    }
	  if (!phasefit_total) phasefit_total = 1;
	  // now wet run..
	  for (unsigned4 i = startshift ; i < stopshift && ! stop_signal; i++ )
	    {
	      unsigned4 phase = i >> blockshifter;
	      unsigned4 store = phase << blockshifter;
	      updateProcessingProgress(phasefit_called*100/phasefit_total);
	      if (store!=i) continue;
	      // we hebben een positie die een macht van 2 is...
	      // dus de vorige was op - en + blocksize *2
	      unsigned4 prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned4 next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (prev_store < startshift ) continue;
	      if (next_store >= stopshift ) break;
	      // sign is important !
	      unsigned4 prev_val = prev_mismatch[prev_store]; 
	      // sign is important !
	      unsigned4 next_val = prev_mismatch[next_store]; 
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		{
		  unsigned4 m = phasefit(phase);  
		  // we moeten de phase kleiner maken omdat de array
		  //  geresampled is op blocksize
		  mismatch[store]=m;
		  phasefit_called++;
		}
	    }
	}
      updateProcessingProgress(100);
      // find minimum, translate, maximum, mean
      signed minimum = mismatch[startshift];
      minimum_at = startshift;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (minimum == -1 || (mismatch[i] >= 0 && mismatch[i] < minimum))
	  minimum = mismatch[ minimum_at = i ];
      minima[blockshifter] = minimum_at;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0)
	  mismatch[i]-=minimum;
      signed maximum = -1;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0 && (mismatch [i] > maximum || maximum < 0)) 
	  maximum = mismatch[i];
      signed8 total = 0;
      signed4 count = 0;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i]>=0)
	  {
	    count++;
	    total+=mismatch[i];
	  }
      maxima[blockshifter]=maximum;
      if (!count) count = 1;
      mean[blockshifter]=total/count;
      
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      p.setPen(Qt::gray);
      p.drawLine(IMAGE_XS/2,0,IMAGE_XS/2,IMAGE_YS);
      p.drawLine(IMAGE_XS/4,0,IMAGE_XS/4,IMAGE_YS);
      p.drawLine(IMAGE_XS*3/4,0,IMAGE_XS*3/4,IMAGE_YS);
      
      for (unsigned4 i = blockshifter_max ; i >= blockshifter ; i --)
	{
	  unsigned4 slice_maximum = maxima[i];
	  if (!slice_maximum) slice_maximum=1;
	  unsigned4 pos = r.height() - ((signed8)mean[i] * (signed8) r.height()
					/ (signed8)slice_maximum);
	  QColor c(0,0,0);
	  signed4 kleur = (i - blockshifter_min) * 128 /
	    (blockshifter_max - blockshifter_min);
	  c.setHsv(kleur, 255, 255);
	  p.setPen(c);
	  p.drawLine(0,pos,r.width(),pos);
	  float8 bpm = (float8)(4.0*60.0*44100.0)/(float8)minima[i];
	  pos = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  p.drawLine(pos,0,pos,r.height());
	  	  
	  for( unsigned4 j = startshift ; j < stopshift ; j ++ )
	    {
	      signed8 y = mismatch_array[i][j-startshift];
	      if (y > 0)
		{
		  y *= (signed8) r.height();
		  y /= (signed8) slice_maximum;
		  y = r.height() - y;
		  bpm = (float8)(4.0*60.0*44100.0)/(float8)j;
		  unsigned4 x = (signed4)((float8)r.width()*(bpm-startbpm)/
					  (stopbpm-startbpm));
		  p.drawPoint(x,y);
		  p.drawPoint(x+1,y);
		  p.drawPoint(x,y+1);
		  p.drawPoint(x+1,y+1);
		}
	    }
	}
      p.end();
      BpmPix->setPixmap(*pm);
      blockshifter --;
    }

  if (stop_signal)
    status("Canceled");
  else
    {
      status("Ready");
      set_measured_period_diskrate("Rayshooting",minimum_at);
    }
}

void BpmAnalyzerDialog::set_measured_period_diskrate(QString technique, signed4 p, 
bool update_on_disk)
{
  set_normalperiod_metarate(diskrate_to_metarate(p),update_on_disk);
}

void BpmAnalyzerDialog::peak_scan()
{
  status("Finding least broken ray based on FFT");
  stopshift=audiorate*60*4/startbpm;
  startshift=audiorate*60*4/stopbpm;
  unsigned4 match[stopshift-startshift];
  for (unsigned4 i = 0 ; i < stopshift - startshift ; i ++)
    match[i]=0;
  peak_fit = bpmdj_allocate(peaks, signed4);
  for (signed4 i = 0 ; i < peaks ; i ++)
    peak_fit[i]=0;
  
  unsigned4 maximum = 0;
  unsigned4 global_minimum = 0;
  signed4 global_minimum_at = -1;
  signed4 fits = 0;
  signed4 total_fits = peaks*49;
  for (signed4 peak = 0 ; peak < peaks && !stop_signal; peak ++)
    {
      if (peak_bpm[peak]==0) continue;
      unsigned4 a = (signed4)((float8)(audiorate*60*4)/(peak_bpm[peak]+0.5));
      unsigned4 b = (signed4)((float8)(audiorate*60*4)/(peak_bpm[peak]-0.5)); 
      if (a<startshift) a = startshift;
      if (a>stopshift) a = stopshift;
      if (b<startshift) b = startshift;
      if (b>stopshift) b = stopshift;
      assert(a<b);
      unsigned4 s = (b-a) / 25;  // 25 tries voor elke peak
      unsigned4 minimum = match[a-startshift] = phasefit(a, global_minimum);
      unsigned4 minimum_at = a;
      unsigned4 m;
      for (unsigned4 i = a ; i < b  && ! stop_signal; i += s)
	{
	  fits++;
	  if (match[i-startshift]) continue;
	  m = phasefit(i, global_minimum);
	  match[i-startshift] = m;
	  if (m>maximum) maximum = m;
	  if (m<minimum)
	    {
	      minimum = m;
	      minimum_at = i;
	      if (minimum < global_minimum || global_minimum_at == -1)
		{
		  global_minimum = minimum;
		  global_minimum_at = minimum_at;
		}
	    }
	  updateProcessingProgress(fits*100/total_fits);
	}
      peak_fit[peak]=minimum_at;
    }

  if (stop_signal)
    {
      status("Canceled during autocorrelation");
      return;
    }
  
  // now we have all peaks, it is time to do a fine scan of the missing 
  // information...
  for (signed4 peak = 0 ; peak < peaks; peak ++)
    {
      if (peak_fit[peak]==0) continue;
      unsigned4 a = peak_fit[peak]-12;
      unsigned4 b = peak_fit[peak]+12;
      if (a<startshift) a = startshift;
      if (a>stopshift) a = stopshift;
      if (b<startshift) b = startshift;
      if (b>stopshift) b = stopshift;
      assert(a<b);
      unsigned4 minimum = match[peak_fit[peak]-startshift];
      unsigned4 m = minimum;
      unsigned4 minimum_at = peak_fit[peak];
      for (unsigned4 i = a ; i < b ; i ++)
	{
	  fits++;
	  if (match[i-startshift]) continue;
	  m = phasefit(i, global_minimum);
	  match[i-startshift] = m;
	  if (m>maximum) maximum = m;
	  if (m<minimum)
	    {
	      minimum = m;
	      minimum_at = i;
	      if (minimum < global_minimum || global_minimum_at == -1)
		{
		  global_minimum = minimum;
		  global_minimum_at = minimum_at;
		}
	    }
	  updateProcessingProgress(fits*100/total_fits);
	}
      peak_fit[peak]=minimum_at;
    }

  if (stop_signal)
    {
      status("Canceled while autocorrelating");
      return;
    }

  // update tempo information 
  processing_progress = 100;
  status("Ready");
  
  set_measured_period_diskrate("Peak Scanning",global_minimum_at*4);

  QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  fft_draw(p,IMAGE_XS,IMAGE_YS, shifter,1.0);
  signed4 X,Y;
  p.setPen(Qt::green);
  p.drawLine(IMAGE_XS/2,0,IMAGE_XS/2,IMAGE_YS-1);
  p.drawLine(IMAGE_XS/4,0,IMAGE_XS/4,IMAGE_YS-1);
  p.drawLine(IMAGE_XS*3/4,0,IMAGE_XS*3/4,IMAGE_YS-1);
  p.setPen(Qt::red);
  
  float8 bpm = (float8)(4*60*audiorate)/(float8)global_minimum_at;
  X = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
  p.drawLine(X,0,X,IMAGE_YS-1);
  p.setPen(Qt::blue);
  for(unsigned4 i = startshift ; i < stopshift ; i++ )
    {
      unsigned4 fout=match[i-startshift];
      if (fout)
	{
	  fout -= global_minimum;
	  bpm = (float8)(4*60*audiorate)/(float8)i;
	  X = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  Y = (signed4)((signed8)fout * (signed8)r.height() / 
			(signed8) (maximum-global_minimum));
	  Y = r.height() - Y;
	  p.drawPoint(X,Y);
	  p.drawPoint(X+1,Y);
	  p.drawPoint(X,Y+1);
	  p.drawPoint(X+1,Y+1);
	}
    }
  p.end();
  BpmPix->setPixmap(*pm);
}

void BpmAnalyzerDialog::stopped()
{
  Analyzer::stopped();
  StartStop->setText("&Start");
  BusyToggle->toggle();
}

void BpmAnalyzerDialog::startStopButton()
{
  toggle();
}

void BpmAnalyzerDialog::started()
{
  StartStop->setText("&Stop");
  BusyToggle->toggle();
  Analyzer::started();
}

void BpmAnalyzerDialog::stop()
{
  status("Canceling...");
  Analyzer::stop();
}

void BpmAnalyzerDialog::finish()
{
  Analyzer::stop();
}

void BpmAnalyzerDialog::reset()
{
  tapcount = 0;
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::tap()
{
  if (++tapcount==1) 
    {
      starttime=times(NULL);
    }
  else
    {
      /**
       * we have to decrease tapcount with one because at tick 2 the 
       * time passed counts for only one beat.
       */
      signed8 p = (((signed8)times(NULL)-(signed8)starttime)/(tapcount-1));
      p *= normalperiod_metarate;
      p /= currentperiod_metarate;
      p /= SkipBox->value();
      p *= 11025*4;
      p /= CLOCK_FREQ;
      
      /**
       * This is tricky. Because we modify the native tempo
       * the player will start playing faster because it tries 
       * to honour the current requested tempo (given by currentperiod). 
       * However, since we are tapping the tempo it is unwanted 
       * that the playing speed changes. Hence, we need to reset
       * the currentperiod so that it plays equally fast after the 
       * change in the native tempo.
       */
      signed8 oldcurrentperiod_metarate=currentperiod_metarate;
      signed8 oldnormalperiod_metarate=normalperiod_metarate;
      set_measured_period_diskrate("Tap measurement",period_to_quad(p),false);
      ::metronome->changetempo(normalperiod_metarate*oldcurrentperiod_metarate/oldnormalperiod_metarate);
      
      tempo_type tempo = playing->get_tempo();
      setBpmBounds(tempo.lower(10),tempo.higher(10));
    }
  TapLcd->display(tapcount);
}
	
/**
 * Technique nr 6:
 */
void BpmAnalyzerDialog::experimental_scan_varia()
{
  FILE * f,* g;
  /** 
   * load the full data into memory, barring the stereo information
   * that is not so very interesting to consider
   * We will work with 6 different fourier analyses, at lengts
   * 1024, 2048, 4096, 8192, 16384 and 32768. This means that we will need to read
   */
  const int largest_window=32768;
  const int stride=1024;
  FILE * raw = openCoreRawFile();
  assert(raw);
  audiosize=fsize(raw)/4;
  if (audiosize<largest_window) 
    {
      status("Not enough data to work with");
      return;
    }
  int block_count=1+(audiosize-largest_window)/stride;
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
      int window_size=window_sizes[widx];
      stereo_sample2 intaudio[window_size];
      double* han=bpmdj_allocate(window_size,double);
      for(int i = 0 ; i < window_size; i++)
	{
	  double x=i*M_PI/(double)window_size;
	  x-=M_PI/2;
	  han[i]=(sin(x)+1)/2;
	}
      double* audio=(fft_type*)fftw_malloc(window_size*sizeof(double));
      double* hcfreq=(fft_type*)fftw_malloc(window_size*sizeof(double));
      fftw_plan plan = fftw_plan_r2r_1d(window_size,audio,hcfreq,FFTW_R2HC,FFTW_MEASURE);
      assert(current_channel+countfreqs[widx]<=channel_count);
      for(int block = 0 ; block < block_count; block++)
	{
	  if (block%10==0)
	    updateReadingProgress(((double)widx+(double)block/(double)block_count)*100.0/6.0);
          // read this audio block
	  unsigned8 start=block;
	  start*=stride;
	  assert(start+window_size<=(unsigned8)audiosize);
	  fseek(raw,4*start,SEEK_SET);
	  int rd=readsamples(intaudio,window_size,raw);
	  assert(rd==window_size);
	  for(int i = 0 ; i < window_size; i++)
	    audio[i]=intaudio[i].left+intaudio[i].right;
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
  fclose(raw);
  
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
  updateReadingProgress(100);
  
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
	  fprintf(g,"%g",60.0*i*diskrate/(stride*block_count));
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
  status("Rayshooting");
  
  /**
   * Now we start the rayshooting. Since we already have a relative large blocksize,
   * we can simply scan everyhing I believe
   * We could also print out the tempo for each of the different channels and
   * see which ones should carry more weight.
   */
  fft_type* mismatch_2nd=(fft_type*)fftw_malloc(block_count*sizeof(fft_type));

  signed8 mismatch_array[block_count];
  for(int i = 0 ; i < block_count; i++)
    {
      mismatch_2nd[i]=0;
      mismatch_array[i]=0;
    }

  /**
   * The start_bpm and stop_bpm should be converted to the best matching period
   * which in this case is in blocks of stride size
   */
  int pscan_stop=((diskrate*60.0/startbpm) // samplecount per beat
		  *4                      // samples per bar
		  /stride);               // blocks per bar
  int pscan_start=((diskrate*60.0/stopbpm) // samplecount per beat
		   *4                     // samples per bar
		   /stride);              // blocks per bar
  if(pscan_start>2) pscan_start--;        // ensure we certainly cover the stopbpm;
  pscan_stop++;                           // ensure we cover startpbpm
  
  /**
   * we double the pscan_stop so that our accuracy increases 
   * in the later phases.
   * So if the lowest tempo is stop_bpm then we must have at
   * least 16 beats at that tempo available. So at 40 BPM 
   * we must have at least 24 seconds music available, and of course 
   * since we must also be able to compare those 16 beats we need
   * around 48 seconds.
   */
  pscan_stop*=2;
  
  /**
   * Todo: print warning if bounds are weird
   */
  if (pscan_stop*2>=block_count) pscan_stop=(block_count/2);
 
  for(int p=1;p<pscan_stop;p++)
    {
      /**
       * Calculate the average mismatch for each offset
       */
      unsigned8 mismatch=0;
      unsigned8 additions=0;
      int i=block_count-p;
      assert(i<block_count);
      while(--i>=0)
	{
	  unsigned1* I=intblocks[i];
	  unsigned1* J=intblocks[i+p];
	  int C=channel_count;
	  while(--C>=0)
	    mismatch+=abs((int)I[C]-(int)J[C]);
	  additions++;
	}
      mismatch/=additions;
      mismatch_array[p]=mismatch;
    }

  double array_exp[block_count];
#ifdef RS_MAX_RAISE
  /**
   * Now, because we want to scim the tops we will always retain and use the last maximum value
   * so this is not a true average as one would expect it. This is a nice solution.
   */
  unsigned8 maxsofar=0;
  for(int p = 0 ; p < pscan_stop; p++)
    {
      unsigned8 v=mismatch_array[p];
      if (v>maxsofar) 
	maxsofar=v;
      mismatch_array[p]=v-maxsofar;
      array_exp[p]=maxsofar;
    }  
#endif

 /**
   * Calculate the mismatcharray on the mismatch_Arry itself and place it in
   * mismatch_2nd. We mus allocate the plan already here, otherwise the content 
   * of the mismatch_array will be overwritten. (That is if the plan is initialized after
   * the data was filled the data is lost)
   */  
  fftw_complex* mismatch_2nd_freq=(fftw_complex*)fftw_malloc(block_count*sizeof(fftw_complex));
  int mismatch2nd_cnt=pscan_stop/2;
  fftw_plan plan = fftw_plan_dft_r2c_1d(mismatch2nd_cnt,mismatch_2nd,mismatch_2nd_freq,FFTW_MEASURE);
  for(int p = 1 ; p < mismatch2nd_cnt; p++)
    {
      unsigned8 mismatch=0;
      unsigned8 additions=0;
      int i=pscan_stop-p;
      while(--i>=0)
	{
	  mismatch+=abs((signed8)mismatch_array[i]-(signed8)mismatch_array[i+p]);
	  additions++;
	}
      mismatch_2nd[p]=(double)mismatch/(double)additions;
      //      printf("%lld\t%lld\t%g\n",mismatch,additions,mismatch_2nd[p]);
    }
  /**
   * The advanatage of this mismatcharray iteration is that we 
   * can fairly confident say that the tempo will be a multiplier of the
   * period found in the resulting array.
   * Which period we need to choose is an entirely different question of 
   * course. So if we need to choose between around 16 different
   * periods that all might be the underlying tempo
   * Once we have the essential idea of the tempo we should be able to 
   * extract all these entries out of the music and sort them in some form
   * of sensible array. How many beats do we have in this case ?
   * If we on average have around 1435 beats in a song then we should be able to extract
   * those single entitites out of it. Well, we could also work with the 18 samples we
   * work with. That would mean that we end up with 
   */ 
#ifdef RS_MAX_RAISE
  /**
   * Now, because we want to scim the tops we will always retain and use the last maximum value
   * so this is not a true average as one would expect it. This is a nice solution.
   */
  double maxsofar2nd=0;
  for(int p = 0 ; p < pscan_stop; p++)
    {
      double v=mismatch_2nd[p];
      if (v>maxsofar2nd) 
	maxsofar2nd=v;
      mismatch_2nd[p]=v-maxsofar2nd;
    } 
#endif

  /**
   * We calculate the frequency content of the 2nd degree rayshoot
   * as to have an indication what the tempo might be. After this
   * step we know that the period is a multiple of the period of a 
   * beat is a multiple of the period with highest energy in this
   * analysis.
   */
  fftw_execute(plan);

  /**
   * The mismatch_freq to tempo mapping
   * - the blockperiod is given by: pscan_stop/bin
   * - this period relies on the stride to know the number of samples: pscan_stop*stride/bin
   * - this sample period can be mapped to seconds through: pscan_stop*stride/(bin*wavrate)
   * - these second can become minutes: pscan_stop*stride/(bin*wavrate*60)
   * - and then into a tempo: bin*wavrate*60/pscan_stop*stride;
   * - so each bin reflects around 9.9BPM if we only use pscan_stop. So we should go further than that
   *   and measure to very low frequencies then our accuracy increases.
   */
  int start_block=60*diskrate/startbpm/stride;
  start_block++;
  int start_bin=mismatch2nd_cnt/start_block;
  
  g=fopen("2ndfreq","wb");
  assert(g);
  /**
   * Print out bins and calculate energy
   */
  for(int i = 1 ; i < mismatch2nd_cnt/2; i++)
    {
      double R=mismatch_2nd_freq[i][0];
      double I=mismatch_2nd_freq[i][1];
      double A=sqrt(R*R+I*I);
      double tempo=i*diskrate*60/mismatch2nd_cnt/stride;      
      fprintf(g,"%g\t%d\t%g\t%g\n",tempo,i,mismatch_2nd[i],A);
      mismatch_2nd[i]=A;
    }
  fclose(g);
  
  /** 
   * Find maximum in the start_bin to stop_bin range
   */
  int freqbin=0;
  max=0;
  for(int i = start_bin ; i < mismatch2nd_cnt/2; i++)
    {
      double tempo=i*diskrate*60/mismatch2nd_cnt/stride;      
      if (tempo<startbpm || tempo>stopbpm) continue;
      double A=mismatch_2nd[i];
      if (A>max)
	{
	  max=A;
	  freqbin=i;
	}
    }
  
  /** 
   * is the freqbin the same as the start_bin, then we must remove the slope
   */
  if (freqbin==start_bin)
    {
      double dA=-1;
      double oA=mismatch_2nd[freqbin];
      do
	{
	  double A=mismatch_2nd[++freqbin];
	  dA=A-oA;
	  oA=A;
	}
      while(dA<=0);
    }

  /**
   * Calculate the tempo and check the bounds
   */
  double tempo=freqbin*diskrate*60/mismatch2nd_cnt/stride;
  if (tempo<startbpm || tempo>stopbpm) tempo=-1;
  printf("Tempo is %g\n",tempo);
  set_measured_period_diskrate("Melodic Rayshooting",(int)((double)diskrate*60.*4./tempo));
  
  f=fopen("mismatch.txt","wb");
  for(int p = 1; p<pscan_stop; p++)
    {
      // the calculation of period to tempo is conducted as follows
      double tempo=p*stride  // samples for 1 measure
	/4;                  // samples per beat
      tempo=(diskrate/tempo)  // beats per second
	*60.0;               // beats per minute
      double v1=mismatch_2nd[p];
      double v2=array_exp[p];
      unsigned8 v4=mismatch_array[p];
      /** 
       * Columns
       * 1 - tempo
       * 2 - period
       * 3 - rayshoot mismatch
       * 4 - rayshoot 2nd batch
       * 5 - array exp
       */
      fprintf(f,"%g\t%d\t%lld\t%g\t%g\n",tempo,p,v4,v1,v2);
    }
  fclose(f);
}
#endif // __loaded__bpm_analyzer_cpp__
