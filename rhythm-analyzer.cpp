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
#ifndef __loaded__rhythm_analyzer_cpp__
#define __loaded__rhythm_analyzer_cpp__
using namespace std;
#include <cstdio>
#include <qapplication.h>
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
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <fftw3.h>
#include <qmessagebox.h>
#include <sys/ioctl.h>
#include <qspinbox.h>
#include <unistd.h>
#include <libgen.h>
#include <qslider.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include "memory.h"
#include "rhythm-analyzer.h"
#include "rhythm-property.h"
#include "bpmplay.h"
#include "version.h"
#include "scripts.h"
#include "pca.h"
#include "clock-drivers.h"

void fft_to_bark(float8 * in_r, int window_size, spectrum_type &out)
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
      out.set_bark(b,0);
      float8 r = 0;
      for(int i = a ; i < c; i++)
	r+=fabs(in_r[i]);
      out.set_bark(b,r/(c-a));
    } 
};

RhythmAnalyzer::RhythmAnalyzer(QWidget*parent) : QWidget(parent)
{
  setupUi(this);
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
  rhythm->setPixmap(*pm);
}

static float8 tovol(float8 a)
{
  float8 r = 10.0*log(a)/log(10.0);
  if (isnan(r)) return 0;
  if (r<-30) return 0;
  return r+30;
}

float8 xy_dist(float8 x1, float8 y1, float8 x2, float8 y2)
{
  float8 a = x1 - x2;
  float8 b = y1 - y2;
  a *= a;
  b *= b;
  return sqrt(a+b);
}

static fftw_plan plan;
static float8 *fft_out = NULL;
static float8 *fft_in = NULL;
static float8 *init_bark_fft2(int window_size)
{
  fft_in  = bpmdj_allocate(window_size,float8);
  fft_out = bpmdj_allocate(window_size,float8);
  plan = fftw_plan_r2r_1d(window_size,fft_in,fft_out,FFTW_R2HC,FFTW_MEASURE);
  return fft_in;
}

static void bark_fft2(int window_size, spectrum_type &bark_energy)
{
  fftw_execute(plan);
  fft_to_bark(fft_out,window_size,bark_energy);
}

static void free_bark_fft2()
{
  fftw_free(plan);
}

void RhythmAnalyzer::calculateRhythmPattern2()
{
  if (!normalperiod_metarate)
    { 
      QMessageBox::critical(NULL,
	    "Rhythm analyzer",
	    QString("Need tempo before rhythm can be analyzed"),
	    QMessageBox::Ok,0,0);
      return;
    }
  // constants and some variables
  const int window_size = 2048;
  const int view_xs = 640;
  // how many frames will go into the period of the song
  int slice_samples = metarate_to_diskrate(normalperiod_metarate * beats->value() / 4);
  int slice_frames = 192;
  // allocate arrays
  stereo_sample2 * slice_audio = bpmdj_allocate(slice_samples,stereo_sample2);
  spectrum_type * slice_freq = bpmdj_allocate(slice_frames,spectrum_type);
  spectrum_type * slice_prot = bpmdj_allocate(slice_frames,spectrum_type);
  spectrum_type * last_measure = bpmdj_allocate(slice_frames,spectrum_type);
  for(int i = 0 ; i < slice_frames ; i ++)
    for(int b = 0 ; b < 24 ; b++)
      {
	slice_prot[i].set_bark(b,0);
	last_measure[i].set_bark(b,0);
      }
  float8 * buffer_fft = init_bark_fft2(window_size);
  // open song
  FILE * raw = openCoreRawFile();
  long int audio_size = fsize(raw)/4;
  int max_slices = audio_size / slice_samples;
  spectrum_type * changes = bpmdj_allocate(max_slices,spectrum_type);
  for(int i = 0 ; i < max_slices ; i ++)
    for(int b = 0 ; b < 24 ; b ++)
      changes[i].set_bark(b,0);
  unsigned4 * phases = bpmdj_allocate(max_slices,unsigned4);
  // go through song slice by slice
  status_bar->setText("Calibrating");
  for(int slice = 0 ; slice < max_slices ; slice++)
    {
      Progress->setMaximum(max_slices);
      Progress->setValue(slice);

      if (slice==1) status_bar->setText("Processing");
      app->processEvents();
      // read data
      long int pos = slice * slice_samples;
      fseek(raw,pos*sizeof(stereo_sample2),SEEK_SET);
      assert(pos+slice_samples <= audio_size);
      int read = readsamples(slice_audio,slice_samples,raw);
      assert(read>0);
      for(int a = 0 ; a < slice_frames ; a ++)
	{
	  // take the appropriate data
	  int s = a * ( slice_samples - window_size )/ slice_frames;
	  assert(s + window_size < slice_samples);
	  for(int i = 0 ; i < window_size ; i++)
	    buffer_fft[i]=slice_audio[i+s].left+slice_audio[i+s].right;
	  // convert to spectrum and normalize
	  bark_fft2(window_size,slice_freq[a]);
	  for(int i = 0 ; i < 24 ; i ++)
	    {
	      float8 v;
	      v = slice_prot[a].get_bark(i);
	      assert(!isinf(v));
	      v = tovol(slice_freq[a].get_bark(i));
	      assert(!isinf(v));
	      slice_freq[a].set_bark(i,v);
	    }
	}
      // find best rotational fit towards prototype
      float8 m = HUGE;
      int a = 0;
      for(int p = 0 ; p < slice_frames ; p++)
	{
	  float8 d = 0;
	  for(int y = 0 ; y < slice_frames ; y++)
	    {
	      int z = (y + p)%slice_frames;
	      for(int b = 0 ; b < 24 && d < m; b ++)
		{
		  float8 v = slice_prot[y].get_bark(b) / slice;
		  float8 w = slice_freq[z].get_bark(b);
		  d += (v-w) * (v-w);
		}
	    }
	  if (d<m)
	    {
	      m = d;
	      a = p;
	    }
	}
      phases[slice]=a * ( slice_samples - window_size )/ slice_frames;
      // add the rotation to the prototype
      for(int y = 0 ; y < slice_frames; y++)
	{
	  int z = (y + a)%slice_frames;
	  for(int b = 0 ; b < 24 ; b++)
	    slice_prot[y].set_bark(b,slice_prot[y].get_bark(b)
				   +slice_freq[z].get_bark(b));
	}
      // find best rotational fit tov last measure
      if (slice>0)
	{
	  // store last measure and remember change level
	  for(int b = 0 ; b < 24 ; b++)
	    {
	      float8 d = 0;
	      for(int y = 0 ; y < slice_frames; y++)
		{
		  // float8 v = last_measure[y].get_bark(b);
		  float8 w = slice_freq[y].get_bark(b);
		  d += /* v - */ w;
		}
	      // 	      d = log(d);
	      if (d<0) d = 0;
	      changes[slice].set_bark(b,d);
	    }
	}
      for(int p = 0 ; p < slice_frames ; p++)
	for(int q = 0 ; q < 24 ; q ++)
	  last_measure[p].set_bark(q,slice_freq[p].get_bark(q));
    }
  
  Progress->setMaximum(1); 
  Progress->setValue(1);
  
  fclose(raw);
    
  status_bar->setText("Normalizing");
  app->processEvents();

  // normalize the spectrum
  for(int f = 0 ; f < slice_frames ; f++)
    for(int b = 0 ; b < 24 ; b ++)
      slice_prot[f].set_bark(b,slice_prot[f].get_bark(b)/max_slices);
  spectrum_type maxima;
  spectrum_type mean;
  spectrum_type scale;
  for(int f = 0 ; f < slice_frames ; f++)
    for(int b = 0 ; b < 24 ; b ++)
      {
	float8 v = slice_prot[f].get_bark(b);
	mean.set_bark(b,mean.get_bark(b)+v);
	if (fabs(v) > maxima.get_bark(b))
	  maxima.set_bark(b,fabs(v));
      }
  for(int f = 0 ; f < slice_frames ; f++)
    for(int b = 0 ; b < 24 ; b ++)
      {
	float8 v = slice_prot[f].get_bark(b);
	float8 m = mean.get_bark(b) / slice_frames;
	v -=m;
	v /= maxima.get_bark(b) - m;
	scale.set_bark(b,maxima.get_bark(b) - m);
	slice_prot[f].set_bark(b,v);
      }

  // now we must find a good start on the music.
  // the start is the place after which most energy is packed 
  // as well now as in the longer future. 
  // so we take 1/16the surface and add it with 1/8the and 1/4th and 1/2
  int startpos = 0;
  float8 startenergy = 0;
  status_bar->setText("Locating start");
  app->processEvents();
  for(int a = 0 ; a < slice_frames ; a++)
    {
      float8 total = 0;
      for(int l = slice_frames/16 ; l < slice_frames ; l*=2)
	{
	  for(int f = 0 ; f < l ; f++)
	    {
	      int z = (a+f)%slice_frames;
	      for(int b = 0 ; b < 24 ; b ++)
		{
		  float8 v = slice_prot[z].get_bark(b);
		  total+=v;
		}
	    }
	}
      if (total>startenergy)
	{
	  startpos=a;
	  startenergy=total;
	}
    }

  // fix the rhythm information for this file
  rhythm_property R;
  R.init();
  for(int y = 0 ; y < 24 ; y++)
    {
      R.set_scale(y,scale.get_bark(y));
      for(int x = 0 ; x < rhythm_prop_sx ; x ++)
	{
	  int z = x * slice_frames / rhythm_prop_sx; 
	  z += startpos;
	  z %= slice_frames;
	  float8 dB = slice_prot[z].get_bark(y);
	  dB*=127;
	  dB+=127;
	  if (dB<0) dB=0;
	  if (dB>255) dB=255;
	  R.set_energy(x,y,(unsigned1)dB);
	}
    }
  playing->set_rhythm(R);

  // now draw the different distribution on the different axis
  QPixmap *pm = new QPixmap(view_xs, 24);
  QPixmap *pr = new QPixmap(view_xs, 1);
  QPainter p, q;
  p.begin(pm);
  q.begin(pr);
  for(int x = 0 ; x < view_xs ; x++)
    {
      int sum = 0;
      for(int y = 0 ; y < 24 ; y ++)
	{
	  int z = x * (slice_frames - 1) / ( view_xs - 1 );
	  z+=startpos;
	  z%=slice_frames;
	  float8 dB = slice_prot[z].get_bark(y);
	  QColor col;
	  /* This is one way of coloring */
	  dB*=192;
	  dB+=63;
	  if (dB<0) dB=0;
	  col.setHsv(y*240/24,255,(int)dB);
	  /* This is another 
	     dB*=255;
	     if (dB>255) dB=255;
	     if (dB<-255) dB=-255;
	     if (dB>=0)
	     col.setRgb((int)dB,0,0);
	     else if (dB<0)
	     col.setRgb(0,0,(int)-dB);
	  */
	  sum+=(int)dB;
	  
	  p.setPen(col);
	  p.drawPoint(x,23-y);
	}
      sum /= 24;
      q.setPen(QColor(sum,sum,sum));
      q.drawPoint(x,0);
    }
  p.end(); 
  q.end();
  rhythm->setPixmap(*pm);
  projection->setPixmap(*pr);
  
  // normalize the composition bar & draw it
  {
    for(int b = 0 ; b < 24 ; b++)
      changes[max_slices-1].set_bark(b,0);
    spectrum_type maxima;
    spectrum_type mean;
    // spectrum_type scale;
    for(int f = 0 ; f < max_slices ; f++)
      for(int b = 0 ; b < 24 ; b ++)
	{
	  float8 v = changes[f].get_bark(b);
	  mean.set_bark(b,mean.get_bark(b)+v);
	  if (fabs(v) > maxima.get_bark(b))
	    maxima.set_bark(b,fabs(v));
	}
    for(int f = 0 ; f < max_slices ; f++)
      for(int b = 0 ; b < 24 ; b ++)
	{
	  float8 v = changes[f].get_bark(b);
	  float8 m = mean.get_bark(b) / max_slices;
	  v -=m;
	  v /= maxima.get_bark(b) - m;
	  // scale.set_bark(b,maxima.get_bark(b) - m);
	  changes[f].set_bark(b,v);
	}
  }

  // draw the composition bar
  pm = new QPixmap(max_slices, 24);
  p.begin(pm);
  p.fillRect(QRect(0,0,max_slices,24),Qt::white);
  p.setPen(Qt::black);
  for(int x = 0 ; x < max_slices ; x++)
    {
      for(int y = 0 ; y < 24 ; y ++)
	{
	  float8 dB = changes[x].get_bark(y);
	  QColor col;
	  dB*=128;
	  dB+=127;
	  if (dB<0) dB=0;
	  col.setHsv(y*240/24,255,(int)dB);
	  p.setPen(col);
	  p.drawPoint(x,23-y);
	}
    }
  p.end(); 
  composition->setPixmap(*pm);

  // calculate the frequency content of the composition
  // we take 33 measures because the first will always be zero
  const int ps = 33; 
  int ws = higher_power_of_two(max_slices);
  if (ps>ws) 
    ws = higher_power_of_two(ps);
  float8 * ain = bpmdj_allocate(ws,float8);
  float8 * aou = bpmdj_allocate(ws,float8);
  float8 * aio = bpmdj_allocate(ws,float8);
  float8 * periods = bpmdj_allocate(ps,float8);
  // int *counts = bpmdj_allocate(ps,int);
  pm = new QPixmap(ps, 24);
  p.begin(pm);
  composition_property cp;
  cp.init();
  for(int y = 0 ; y < 24 ; y ++)
    {
      for(int x = 0 ; x < max_slices ; x++)
	ain[x]=changes[x].get_bark(y);
      for(int x = max_slices - 1; x >=1 ; x--)
	ain[x]-=ain[x-1];
      ain[0]=0;
      for(int x = max_slices - 1; x < ws ; x++)
	ain[x]=0;
      unbiased_autocorrelation(ain,ws);
      for(int pml = 0 ; pml < ps ; pml++)
	periods[pml]=ain[pml];
      periods[0]=0;
      float8 maxed = normalize_abs_max(periods,ps);
      // printf("%g\n",maxed);
      // assign the autocorrelation to the composition property
      cp.set_scale(y,maxed);
      for(int x = 1 ; x < ps ; x ++)
	{
	  float8 val = periods[x];
	  val*=127;
	  val+=127;
	  if (val<0) val = 0;
	  if (val>255) val = 255;
	  cp.set_energy(x-1,y,(unsigned1)val);
	}

      for(int x = 0 ; x < ps ; x++)
	{
	  float8 dB = periods[x];
	  QColor col;
	  if (dB<0) dB=-dB;
	  dB*=255;
	  col.setHsv(y*240/24,255,(int)dB);
	  p.setPen(col);
	  p.drawPoint(x,23-y);
	}
    }
  // the data which we will write out in the index file are the non 
  // absolute periods
  bpmdj_deallocate(aio);
  bpmdj_deallocate(aou);
  bpmdj_deallocate(ain);
  p.end(); 
  composition_freq->setPixmap(*pm);
  playing->set_composition(cp);
  
  status_bar->setText("Done");
  free_bark_fft2();
}
#endif // __loaded__rhythm_analyzer_cpp__
