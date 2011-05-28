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
#ifndef __loaded__beatgraph_analyzer_cpp__
#define __loaded__beatgraph_analyzer_cpp__
using namespace std;
#line 1 "beatgraph-analyzer.c++"
#include <cstdio>
#include <Qt/qapplication.h>
#include <Qt/qlineedit.h>
#include <Qt/qpixmap.h>
#include <Qt/qpainter.h>
#include <Qt/qpushbutton.h>
#include <Qt/qlcdnumber.h>
#include <Qt/qcheckbox.h>
#include <Qt/qlabel.h>
#include <Qt/qprogressbar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <Qt/qprogressdialog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <Qt/qspinbox.h>
#include <unistd.h>
#include <Qt/qcombobox.h>
#include <libgen.h>
#include <Qt/qslider.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <sys/mman.h>
#include <Qt/qmessagebox.h>
#include <set>
#include "beatgraph-label.h"
#include "beatgraph-analyzer.h"
#include "rhythm-analyzer.h"
#include "sys/times.h"
#include "bpmplay.h"
#include "version.h"
#include "scripts.h"
#include "memory.h"
#include "signals.h"
#include "clock-drivers.h"
#include "hues.h"
#include "info.h"

#define COLLAPSE 4
const int maxslice = 8;

BeatGraphAnalyzer::BeatGraphAnalyzer(QWidget * parent, const char* name) :
  QWidget(parent)
{
  setWindowTitle(name);
  setupUi(this);
  period_diskrate = 0;
  audiosize = 0;
  data = NULL;
  bank = NULL;
  hue = NULL;
  signed_data = NULL;
}

void BeatGraphAnalyzer::activate()
{
  getTempo();
  changeVisualisation();
  pattern->activate();
}

void BeatGraphAnalyzer::deactivate()
{
  pattern->deactivate();
}

/**
 * This function reads the data into memory.
 */
void BeatGraphAnalyzer::readFileSigned()
{
  // read in memory and shrink it 
  FILE * raw=openCoreRawFile();
  if (!raw) return;
  long newaudiosize=fsize(raw)/4;
  if (newaudiosize!=audiosize)
    {
      if (signed_data) bpmdj_deallocate(signed_data);
      if (data) bpmdj_deallocate(data);
      if (bank) 
	{
	  for(int i = 0 ; i <= maxslice ; i++)
	    bpmdj_deallocate(bank[i]);
	  bpmdj_deallocate(bank);
	}
      signed_data = NULL;
      data = NULL;
      bank = NULL;
      audiosize=newaudiosize;
    }
  if (signed_data || !audiosize) 
    {
      fclose(raw);
      return;
    };
  signed_data = bpmdj_allocate(audiosize/COLLAPSE,uncompressed);
  
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  long int bufsize = 65536;
  stereo_sample2 buffer[bufsize];

  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples(buffer,toread,raw);
      for(int i = 0 ; i < count/COLLAPSE; i++)
	{
	  uncompressed sum=0;
	  for(int j = 0 ; j < COLLAPSE ; j++)
	    {
	      assert(i*COLLAPSE+j<count);
	      sum+=buffer[i*COLLAPSE+j].left;
	      sum+=buffer[i*COLLAPSE+j].right;
	    }
	  sum /= COLLAPSE*2; // *2 because we take both channels
	  assert(i + pos/COLLAPSE < audiosize/COLLAPSE);
	  signed_data[i+pos/COLLAPSE]=sum;
	}
      pos+=count;
    }

  fclose(raw);

  // normalize the audio
  uncompressed maximum = 0;
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    if (signed_data[i]>maximum)
      maximum=signed_data[i];
  // printf("Reading audio %ld with maximum = %d\n",audiosize,(int)maximum);
  if (maximum==0) return;
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    signed_data[i]/=maximum;
}

void BeatGraphAnalyzer::showEnergyPattern()
{
  unsigned4 collapsed_period = period_diskrate  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int samples_per_column = collapsed_period * COLLAPSE;
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  QImage pm(window_xsize,window_ysize,QImage::Format_RGB32);
  QPainter p;
  p.begin(&pm);
  float4 yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      for(int row = 0 ; row < window_ysize ; row++)
	{
	  unsigned4 idx2 = (int)((float4)row*yscale);
	  assert(idx+idx2<collapsed_size);
	  int val = data[idx+idx2];
	  p.setPen(QColor(val,val,val));
	  p.drawPoint(column,row);
	}
    }
  p.setPen(QColor(0,128,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end();
  pattern->setImage(pm,samples_per_column);
}

void getBandColor(int band, QColor &color, float4 val)
{
  if (band<0)
    color.setRgb(0,0,0);
  else
    {
      float4 p = 240.0*(float4)band/(float4)maxslice;
      color.setHsv((int)p,255, 255);
      int r,g,b;
      color.getRgb(&r,&g,&b);
      float4 d;
      if (p<120)
	d = r*r+g*g;
      else
	d = g*g+b*b;
      d = 255.0*(val > 1.0 ? 1 : val) / sqrt(d);
      r=(int)(((float4)r)*d);
      g=(int)(((float4)g)*d);
      b=(int)(((float4)b)*d);
      color.setRgb(r,g,b);
    }
}

void BeatGraphAnalyzer::calculateEnergy()
{
  if (data) return;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  data = bpmdj_allocate(collapsed_size,compressed);
  
  unsigned fs = 256;  //  fs = collapsed_period / 16;
  float8 me = 0;
  bpmdj_array(bt,collapsed_size,float4);
  for(unsigned4 i = 0 ; i < collapsed_size ; i++)
    bt[i]=0;
  // we take the mean of everything
  bpmdj_array(rr,fs,float4);
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  // we measure energy content
  for(unsigned4 x = 0 ; x < collapsed_size ; x ++)
    {
      bt[x]=signed_data[x]; // -bt[x];
      bt[x]*=bt[x];
    }
  // we sum the area and sqrt it
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  float8 S = 0;
  for(unsigned4 x = 0 ; x < collapsed_size ; x ++)
    {
      S-=rr[x%fs];
      rr[x%fs]=bt[x];
      S+=bt[x];
      float8 R = sqrt(S);
      bt[x>=fs ? x - fs : 0 ] = R;
      if (R>me) me=R;
    }
  bpmdj_deallocate(rr);
  
  for(unsigned4 x = 0 ; x < collapsed_size ; x ++)
    data[x]=(unsigned1)(bt[x]*255.0/me);
  
  // now we have the joy to run through the entire set and select the first
  //  maximum
  /*  int cs = collapsed_period / 4;
      printf("ms\tintensity\n---in steps of %d-----------\n",cs);
      for(unsigned4 x = 0 ; x < collapsed_size - cs ; x += cs)
      {
      int z = x+cs-1;
      float8 m = bt[z];
      for(int y = cs - 1 ; y >= 0 ; y--)
      if (bt[x+y]>=m) 
      m=bt[z=x+y];
      for(int y = 0 ; y < 100 ; y++)
      data[z+y]=(100-y)*255/100;
      printf("%g\t%g\n",(float8)z*((float8)COLLAPSE*1000.0/(float8)WAVRATE),
      bt[z]/me);
      }*/
  bpmdj_deallocate(bt);
}

void BeatGraphAnalyzer::calculateHaar()
{ 
  if (bank) return;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  /**
   * First we calculate a number of layers, based on the audio-stream...
   * In every step we will modify the signed_data set by subtracting the
   * current mean...
   */
  bank = bpmdj_allocate(maxslice+1,float4*);
  bpmdj_array(bank_energy,maxslice+1,float8);
  bool power = false;
  for(int filter = maxslice ; filter >= 0 ; filter --)
    {
      // calculate content of entry 'filter' of filter bank 'bank'
      int window_size = 1 << filter;
      int haar_size = collapsed_size / window_size;
      float4 * filtered = NULL;
      filtered = bank[filter] = bpmdj_allocate(haar_size+1, float4);
      if (!filter)
	{
	  filtered = bank[filter] = bpmdj_allocate(haar_size+1, float4);
	  filtered[haar_size]=0;
	  for(unsigned4 i = 0 ; i < collapsed_size ; i++)
	    filtered[i]=signed_data[i];
	}
      else
	{
	  filtered = bank[filter] = bpmdj_allocate(haar_size+1, float4);
	  filtered[haar_size]=0;
	  if (!power)
	    for(int y = 0 ; y < haar_size ; y ++)
	      {
		int d = y * window_size;
		float4 mean = 0;
		for(int x = 0 ; x < window_size ; x++)
		  mean += signed_data[x+d];
		mean /= (float4)window_size;
		filtered[y] = mean;
		for(int x = 0 ; x < window_size ; x++)
		  signed_data[x+d]-=mean;
	      }
	  else // to show the power we must calculate the block content 
	    // differently
	    for(int y = 0 ; y < haar_size ; y ++)
	      {
		int d = y * window_size;
		float4 mean = 0;
		float4 power = 0;
		for(int x = 0 ; x < window_size ; x++)
		  {
		    float4 v = signed_data[x+d];
		    mean += v;
		    power += v*v;
		  }
		mean /= (float4)window_size;
		power /= (float4)window_size;
		power = sqrt(power);
		filtered[y] = power;
		for(int x = 0 ; x < window_size ; x++)
		  signed_data[x+d]-=mean;
	      }
	}
      
      for(int y = 0 ; y < haar_size ; y++)
	filtered[y]=fabs(filtered[y]);
      
      // find the mean of this bank entry
      // RMS power of this bank = square root of the mean of the squared 
      // amplitude
      float8 mean=0;
      float8 max =0;
      float8 power = 0;
      for(int y = 0 ; y < haar_size ; y++)
	{
	  float4 v = filtered[y];
	  mean += v;
	  power += v*v;
	  if (v>max)
	    max = v;
	}
      mean/=haar_size;
      power = sqrt(power/haar_size);
      // printf("power of bank %d is %g\n",filter,power);
      
      // find the std deviation of this bank entry
      float8 dev = 0;
      int devcnt = 0;

      bank_energy[filter] = 0;
      for(int row = 0 ; row < haar_size ; row++)
	{
	  float4 v = filtered[row];
	  bank_energy[filter]+=v;
	  dev+=fabs(v-mean);
	  devcnt++;
	}
      bank_energy[filter]*=window_size;
      dev/=devcnt;

      // printf("Slice %d has mean energy of %g and deviation of %g and max 
      // of %g\n",filter,mean,dev,max);
      if (dev>0)
	for(int y = 0 ; y < haar_size ; y++)
	  {
	    float4 v = filtered[y];
	    v-=mean;
	    v/=dev;
	    v+=0.5;
	    if (v>1) v=1;
	    if (v<0) v=0;
	    filtered[y]=v;
	  }
    }
  
  // normalize bank_energy
  float8 maxe=0;
  for(int i = 0 ; i <= maxslice ; i++)
    if (bank_energy[i]>maxe)
      maxe=bank_energy[i];
  if (maxe>0)
    for(int i = 0 ; i <= maxslice ; i++)
      bank_energy[i]/=maxe;
  //  for(int i = 0 ; i <= maxslice ; i++)
  //    printf("bank entry %d has %g energy\n",i,bank_energy[i]);
}


/**
 * ws = the window size over which we calculate the rms value
 * n = the length of the input and output channels
 * data = the start of the input and output channel (This is an in-place 
 *        transform) 
 * stride = how to go from one sample to the next. 1 is a standard array. 2 
 *          will skip 1
 *
 * This function does what I hate in biological data analysis. It THRESHOLDS. 
 * The horror but it works quite well. I would never do this if it had not to 
 * do with the speed of computation. Whoever reads this: please forgive me for 
 * this random collection of thresholds. Well.... They are not that random. The
 * mean is move to 0 and then the average distance to the mean is used to 
 * reposition the mean.
 *
 * A second disaster with this function is that it receives an area and treats 
 * it as signed4 integers and then write the result back as if it were 
 * unsigned4 !
 */
void channel_to_energy(unsigned4 ws, unsigned4 n, void*D, unsigned4 stride)
{
  assert(ws<=n);
  assert(ws>0);

  /**
   * First we convert the signed integers to unsigned integers that can be 
   * added ws times and we also square them already
   */
  {
    signed4* cur_i=(signed4*)D;
    signed4* sa=cur_i+n*stride;
    signed8 Mx=0;
    while(cur_i<sa)
      {
	if (abs(*cur_i)>Mx)
	  Mx=abs(*cur_i);
	cur_i+=stride;
      }
    cur_i=(signed4*)D;
    while(cur_i<sa)
      {
	signed8 v=*cur_i; 
	// v*v has a range 0 to Mx*Mx
	// we want it in range 0..65535/ws
	v*=v;
	v/=Mx;
	v*=65535;
	v/=Mx*ws;
	*(unsigned4*)cur_i=v;
	cur_i+=stride;
      }
  }
  
  unsigned4 S = 0;
  unsigned4* cur_u=(unsigned4*)D;
  unsigned4* sa=cur_u+ws*stride;
  unsigned8 me = 0; 
  while(cur_u<sa)
    {
      signed8 v=*cur_u;
      S+=v;
      me+=S;
      cur_u+=stride;
    }
  unsigned4* target=(unsigned4*)D;
  assert(cur_u==target+ws*stride);
  assert(n<65536);
  for(unsigned4 x = ws ; x < n ; x ++)
    {
      S-=*target;
      S+=*cur_u;
      cur_u+=stride;
      *target=S;
      target+=stride; 
      me+=S;
    }
  me/=n;
  
  unsigned4 Me=me;
  unsigned4* cur=(unsigned4*)D;
  unsigned4* saf=cur+n*stride;
  unsigned4 mx=0;
  while(cur<saf)
    {
      /** 
       * To normalize the signal here we tested a number of different approaches
       * subtraction of the mean; division by the mean and log(v/me) 
       * there was not much qualitative difference between subtraction and log,
       * except that we
       * had gray faster than with subtraction. Division underperformed
       // v/=me; // division method
       // v=log(v/me);  // log method
       */
      unsigned4 v=*cur;
      if (v<Me) *cur=0;
      else
	{
	  v-=Me;
	  if (v>mx) mx=v;
	  *cur=v;
	}
      cur+=stride;
    }
  cur=(unsigned4*)D;
  while(cur<saf)
    {
      *(unsigned4*)cur=*cur*255.0/mx;
      cur+=stride;
    }
}

void BeatGraphAnalyzer::calculateF1()
{
  Debug("Beatgraph-F1: Calculating standard energybands");
  calculateEnergy();
  if (hue) return;
  Debug("Beatgraph-F1: First band");
  const int maxlevel=9;
  unsigned4 collapsed_size = audiosize / COLLAPSE;
  /**
   * Beware: In this function multi refers to the data in float format
   * however during the computation of the wavelet transform we 
   * only work with integers due to the efficiency it provides
   * the conversion from integer to float is later performed in the
   * calculate_energy function
   */
  float4* multi_f = bpmdj_allocate(collapsed_size,float4);
  {
    signed4* multi_i = (signed4*)multi_f;
    // this is an optimization by directly calculating the first decomposition
    // we don't need to read the same variable again afterward. This saves an 
    // entire scan through the song. Normally we would simply write
    //
    // | for(unsigned4 i = 0 ; i < collapsed_size; i++)
    // |   multi[i]=signed_data[i];
    //
    // however now we do it as
    
    uncompressed* stopat=signed_data+collapsed_size-1;
    uncompressed* re=signed_data;
    signed4* wr=multi_i;
    while(re < stopat)
      {
	signed4 a=*(re++)*16384.;
	signed4 b=*(re++)*16384.;
	*(wr++)=a+b;
	*(wr++)=a-b;
      }
    
    /**
     * I think this is the shortest wavelet decomposition I wrote so far, 
     * this could be useful to integrate into the old wavelet decomposer. 
     * Although with the current optimization it became somewhat less readable.
     */
    Debug("Beatgraph-F1: Remaining frequency bands");
    for(unsigned1 level = 1 ; level<maxlevel; level++)
      {
	unsigned2 stride= 1<<level; 
	for(unsigned4 j = 0 ; j < stride ; j++)
	  {
	    // Normal:
	    // for(unsigned4 i = j ; i+stride < collapsed_size; i+=2*stride)
	    //   {
	    //   uncompressed a=multi[i];
	    //   uncompressed b=multi[i+stride];
	    //   multi[i]=a+b;
	    //   multi[i+stride]=a-b;
	    //   }
	    // 
	    // Optimized:	 
	    signed4* d1=multi_i+j;
	    signed4* d2=multi_i+j+stride;
	    signed4* sa=multi_i+collapsed_size;
	    unsigned2 dstride=stride*2;
	    while(d2<sa)
	      {
		signed4 a=*d1; // fetch
		signed4 b=*d2; // fetch 
		*d1=a+b;  // 1 flop, store
		*d2=b-a;  // 1 flop, store
		d1+=dstride; // 1 flop
		d2+=dstride; // 1 flop
	      }
	  }
      }
  }
  
  unsigned2 channelcount=1<<maxlevel;
  unsigned4 channelsize=collapsed_size/channelcount;
  unsigned4* channels[channelcount];
  // to map the band to a sorted channel we need to reverse the index bits
  for(int i = 0 ; i < channelcount ; i++)
    {
      int j=0;
      int t=i;
      for(int k=0; k < maxlevel; k++)
	{
	  j<<=1;
	  j+=t%2;
	  t>>=1;
	}
      channels[i]=(unsigned4*)multi_f+j;
    }
  
  unsigned4 normalperiod_diskrate=normalperiod_metarate*diskrate/metarate;
  unsigned4 ws = normalperiod_diskrate/(COLLAPSE*4*(1<<maxlevel));
  if (ws<1) ws=1;
  Debug("Beatgraph-F1: Energy calculations using a windowsize of %d",ws);
  for(int i = 0 ; i < channelcount ; i++)
    channel_to_energy(ws,channelsize,channels[i],channelcount);
  
  Debug("Beatgraph-F1: Distances");
  float4 channel_energy[channelcount];
  for(int tofill = 1 ; tofill < channelcount; tofill++)
    {
      unsigned4* prev=channels[tofill-1];
      unsigned8 dist=0;
      unsigned4* cur=channels[tofill];
      unsigned4 last=channelsize*channelcount;
      for(unsigned4 y = 0 ; y < last; y+=channelcount)
	{
	  unsigned4 cy=cur[y];
	  unsigned4 py=prev[y];
	  if (cy>py)
	    dist+=cy-py;
	  else
	    dist+=py-cy;
	}
      channel_energy[tofill]=sqrt(dist);
    }
  
  float8 total_energy=0;
  for(int i = 0 ; i < channelcount; i++)
    total_energy+=channel_energy[i];
  
  float8 energy_so_far=0;
  unsigned1 channel2hue[1<<maxlevel];
  for(int i = 0 ; i < channelcount; i++)
    {
      channel2hue[i]=(float4)i*240.0/channelcount;
      energy_so_far+=channel_energy[i];
    }
  
  Debug("Beatgraph-F1: Setting hues");
  hue = bpmdj_allocate(collapsed_size,compressed);
  sat = bpmdj_allocate(collapsed_size,compressed);
  signed4 oj=-1;
  unsigned4 m=0;
  int ma=0;
  unsigned4 S=0;
  for(unsigned4 i = 0 ; i < collapsed_size ; i++)
    {
      // here we need to determine which wavepacket we need
      signed4 j=(i/channelcount)*channelcount;
      if (j!=oj)
	{
	  ma=0;
	  m=0;
	  S=0;
	  for(int k=0;k<channelcount; k++)
	    {
	      unsigned4 v=channels[k][j];
	      S+=v;
	      if (v>m)
		{
		  ma=k;
		  m=v;
		}
	    }
	  oj=j;
	}
      /**
       * An interesting side effect from all these calculations is that we can
       * see how many sounds we have at the same time
       */
      sat[i]=S/channelcount;
      hue[i]=channel2hue[ma];
    }
  Debug("Beatgraph-F1: Finished");
}

void BeatGraphAnalyzer::showHaarPattern()
{ 
  unsigned4 collapsed_period = period_diskrate  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int samples_per_column = collapsed_period * COLLAPSE;
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  QImage pm(window_xsize,window_ysize,QImage::Format_RGB32);
  QPainter p;
  p.begin(&pm);
  // show
  float4 yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      int co = column * collapsed_period;
      for(int row = 0 ; row < window_ysize ; row++)
	{
	  QColor c;
	  int ro = co + (int)((float4)row*yscale);
	  int r = 0, g = 0, b = 0;
	  for(int slice = maxslice; slice>=0 ; slice --)
	    {
	      int x1 = ro >> slice;
	      int x2 = (ro+1) >> slice;
	      float4 value = 0;
	      if (x2>x1)
		for(int x = x1 ; x < x2 ; x ++)
		  value += bank[slice][x] / (float4)(x2 - x1);
	      else
		value = bank[slice][x1];
	      getBandColor(maxslice-slice,c,value);
	      r+=c.red();
	      g+=c.green();
	      b+=c.blue();
	    }
	  r*=2;
	  g*=2;
	  b*=2;
	  r/=maxslice+1;
	  g/=maxslice+1;
	  b/=maxslice+1;
	  if (r>255) r = 255;
	  if (g>255) g = 255;
	  if (b>255) b = 255;
	  c.setRgb(r,g,b);
	  p.setPen(c);
	  p.drawPoint(column,row);
	}
    }

  p.setPen(QColor(0,128,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setImage(pm,samples_per_column);
}

void BeatGraphAnalyzer::showF1Pattern()
{ 
  unsigned4 collapsed_period = period_diskrate  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int samples_per_column = collapsed_period * COLLAPSE;
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  QImage pm(window_xsize,window_ysize,QImage::Format_RGB32);
  QPainter p;
  p.begin(&pm);
  float4 yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      QColor C;
      for(int row = 0 ; row < window_ysize ; row++)
	{
	  unsigned4 idx2 = (int)((float4)row*yscale);
	  assert(idx+idx2<collapsed_size);
	  int val = data[idx+idx2];
	  int h = hue[idx+idx2];
	  int s2 = sat[idx+idx2];
	  int s1 = 255-s2;
	  assert(h>=0 && h<huecolors);
	  QRgb c=get_hue(h,val);
	  // add a specific amount of gray 
	  c=qRgb((qRed(c)*s1+val*s2)/255,
		 (qGreen(c)*s1+val*s2)/255,
		 (qBlue(c)*s1+val*s2)/255);
	  pm.setPixel(column,row,c);
	}
    }
  p.setPen(QColor(0,128,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setImage(pm,samples_per_column);
}

bool BeatGraphAnalyzer::check_visualisation_conditions(bool file_read)
{
  if (!period_diskrate)
    {
      QMessageBox::warning(this,"No period estimate",
	   "No period estimate, hence cannot show the beat graph.\n"
	   "Please go to the BPM counter and measure the tempo first");
      return false;
    }
  if (!audiosize && file_read)
    {
      QMessageBox::warning(this,"Fragment too small",
	   "There is simply no raw data on disk,\n"
	   "Hence, I can't display the beat graph");
      return false;
    }
  return true;
}

void BeatGraphAnalyzer::showPattern()
{
  if (!check_visualisation_conditions()) return;
  if (bg_haar->isChecked())
    showHaarPattern();
  else if (bg_f1->isChecked())
    showF1Pattern();
  else
    showEnergyPattern();
  update();
}

/**
 * This function must be called when the visualization mode changes
 */
void BeatGraphAnalyzer::changeVisualisation()
{
  if (!check_visualisation_conditions(false)) return;
  readFileSigned(); 
  if (!check_visualisation_conditions()) return;
  if (bg_haar->isChecked())
    calculateHaar();
  else if (bg_f1->isChecked())
    calculateF1();
  else
    calculateEnergy();
  showPattern();
}

void BeatGraphAnalyzer::getTempo()
{
  if (!normalperiod_metarate.valid()) period_diskrate = 0;
  else period_diskrate=(normalperiod_metarate+periodDelta->value()+periodDelta10->value())*diskrate/metarate;
  if (period_diskrate<=0) period_diskrate=0;
}

void BeatGraphAnalyzer::slantChanged()
{
  getTempo();
  showPattern();
}

void BeatGraphAnalyzer::setTempo()
{
  if (!playing) return;
  bool was_normal = normalperiod_metarate == currentperiod_metarate;
  bool was_target = targetperiod_metarate == currentperiod_metarate;
  assert(metronome);
  signed8 newnormalperiod=normalperiod_metarate+periodDelta->value()+
    periodDelta10->value();
  if (newnormalperiod<=0) newnormalperiod = 1;
  set_normalperiod_metarate(newnormalperiod);
  periodDelta->setValue(0);
  periodDelta10->setValue(0);
  if (was_target) emit targetTempo();
  if (was_normal) emit normalTempo();
  pattern->setFocus();
}

void BeatGraphAnalyzer::cuesChanged()
{
  pattern->cues_changed=true;
  pattern->update();
}
#endif // __loaded__beatgraph_analyzer_cpp__
