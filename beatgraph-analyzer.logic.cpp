/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#include <qprogressdialog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <qspinbox.h>
#include <unistd.h>
#include <qcombobox.h>
#include <libgen.h>
#include <qslider.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <sys/mman.h>
#include <qmessagebox.h>
#include "beatgraph-analyzer.logic.h"
#include "rythm-analyzer.logic.h"
#include "sys/times.h"
#include "kbpm-play.h"
#include "fourier.h"
#include "version.h"
#include "scripts.h"
#include "memory.h"
#include "signals.h"

#define COLLAPSE 4
const int maxslice = 8;
// maxslice 10 with HAAR_COLLAPSE 4: blokes van 1024 samples, samplerate van 11025, measured frequency = 10 Hz
// maxslice 8  with HAAR_COLLAPSE 4: blokes van 256 samples, samplerate van 11025, measured frequency = 40 Hz

BeatGraphAnalyzer::BeatGraphAnalyzer(QWidget * parent, const char* name) :
  BeatGraphWidget(parent,name)
{
  period = 0;
  audiosize = 0;
  data = NULL;
  bank = NULL;
  signed_data = NULL;
}

void BeatGraphAnalyzer::activate()
{
  getTempo();
  changeVisualisation();
}

/**
 * This function reads the data into memory.
 */
void BeatGraphAnalyzer::readFileSigned()
{
  // read in memory and shrink it 
  FILE * raw=openRawFile(playing,arg_rawpath);
  long newaudiosize=fsize(raw)/4;
  if (newaudiosize!=audiosize)
    {
      if (signed_data) deallocate(signed_data);
      if (data) deallocate(data);
      if (bank) 
	{
	  for(int i = 0 ; i <= maxslice ; i++)
	    deallocate(bank[i]);
	  deallocate(bank);
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
  signed_data = allocate(audiosize/COLLAPSE,uncompressed);
  
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  long int bufsize = 65536;
  stereo_sample2 buffer[bufsize];

  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      for(int i = 0 ; i < count/COLLAPSE; i++)
	{
	  uncompressed sum=0;
	  for(int j = 0 ; j < COLLAPSE ; j++)
	    {
	      assert(i*COLLAPSE+j<count);
	      sum+=buffer[i*COLLAPSE+j].left;
	      sum+=buffer[i*COLLAPSE+j].right;
	    }
	  sum /= COLLAPSE*2;
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


/**
 * This function shows the energy pattern stored in the data field.
 */
void BeatGraphAnalyzer::showEnergyPattern()
{ 
  unsigned4 collapsed_period = period  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      if (::x/(normalperiod) == column)
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    p.setPen(QColor(val,0,0));
	    p.drawPoint(column,row);
	  }
      else
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
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
  pattern->setPixmap(*pm);
}

void getBandColor(int band, QColor &color, float val)
{
  if (band<0)
    color.setRgb(0,0,0);
  else
    {
      float p = 240.0*(float)band/(float)maxslice;
      color.setHsv((int)p,255, 255);
      int r,g,b;
      color.getRgb(&r,&g,&b);
      float d;
      if (p<120)
	d = r*r+g*g;
      else
	d = g*g+b*b;
      d = 255.0*(val > 1.0 ? 1 : val) / sqrt(d);
      r=(int)(((float)r)*d);
      g=(int)(((float)g)*d);
      b=(int)(((float)b)*d);
      color.setRgb(r,g,b);
    }
}

void BeatGraphAnalyzer::calculateEnergy()
{
  if (data) return;
  unsigned4 collapsed_period = period  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  data = allocate(collapsed_size,compressed);
  
  unsigned fs = 256;  //  fs = collapsed_period / 16;
  double me = 0;
  array(bt,collapsed_size,float);
  for(unsigned4 i = 0 ; i < collapsed_size ; i++)
    bt[i]=0;
  // we take the mean of everything
  array(rr,fs,float);
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  double S = 0;
  // we measure energy content
  for(unsigned4 x = 0 ; x < collapsed_size ; x ++)
    {
      bt[x]=signed_data[x]; // -bt[x];
      bt[x]*=bt[x];
    }
  // we sum the area and sqrt it
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  S = 0;
  for(unsigned4 x = 0 ; x < collapsed_size ; x ++)
    {
      S-=rr[x%fs];
      rr[x%fs]=bt[x];
      S+=bt[x];
      double R = sqrt(S);
      bt[x>=fs ? x - fs : 0 ] = R;
      if (R>me) me=R;
    }
  deallocate(rr);
  
  for(unsigned4 x = 0 ; x < collapsed_size ; x ++)
    data[x]=(unsigned1)(bt[x]*255.0/me);
  
  // now we have the joy to run through the entire set and select the first maximas
  int cs = collapsed_period / 4;
  printf("ms\tintensity\n---in steps of %d-----------\n",cs);
  for(unsigned4 x = 0 ; x < collapsed_size - cs ; x += cs)
    {
      int z = x+cs-1;
      double m = bt[z];
      for(int y = cs - 1 ; y >= 0 ; y--)
	if (bt[x+y]>=m) 
	  m=bt[z=x+y];
      for(int y = 0 ; y < 100 ; y++)
	data[z+y]=(100-y)*255/100;
      printf("%g\t%g\n",(double)z*((double)COLLAPSE*1000.0/(double)WAVRATE),bt[z]/me);
    }
  deallocate(bt);
}

void BeatGraphAnalyzer::calculateHaar()
{ 
  if (bank) return;
  unsigned4 collapsed_period = period  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  // First we calculate a number of layers, based on the audio-stream...
  // In every step we will modify the signed_data set by subtracting the
  // current mean...
  bank = allocate(maxslice+1,float*);
  array(bank_energy,maxslice+1,double);
  bool power = false;
  for(int filter = maxslice ; filter >= 0 ; filter --)
    {
      // calculate content of entry 'filter' of filterbank 'bank'
      int window_size = 1 << filter;
      int haar_size = collapsed_size / window_size;
      float * filtered = NULL;
      filtered = bank[filter] = allocate(haar_size+1, float);
      if (!filter)
	{
	  filtered = bank[filter] = allocate(haar_size+1, float);
	  filtered[haar_size]=0;
	  for(int i = 0 ; i < collapsed_size ; i++)
	    filtered[i]=signed_data[i];
	}
      else
	{
	  filtered = bank[filter] = allocate(haar_size+1, float);
	  filtered[haar_size]=0;
	  if (!power)
	    for(int y = 0 ; y < haar_size ; y ++)
	      {
		int d = y * window_size;
		float mean = 0;
		for(int x = 0 ; x < window_size ; x++)
		  mean += signed_data[x+d];
		mean /= (float)window_size;
		filtered[y] = mean;
		for(int x = 0 ; x < window_size ; x++)
		  signed_data[x+d]-=mean;
	      }
	  else // to show the power we must calculate the block content differently
	    for(int y = 0 ; y < haar_size ; y ++)
	      {
		int d = y * window_size;
		float mean = 0;
		float power = 0;
		for(int x = 0 ; x < window_size ; x++)
		  {
		    float v = signed_data[x+d];
		    mean += v;
		    power += v*v;
		  }
		mean /= (float)window_size;
		power /= (float)window_size;
		power = sqrt(power);
		filtered[y] = power;
		for(int x = 0 ; x < window_size ; x++)
		  signed_data[x+d]-=mean;
	      }
	}
      
      for(int y = 0 ; y < haar_size ; y++)
	filtered[y]=fabs(filtered[y]);
      
      // find the mean of this bank entry
      // RMS power of this bank = square root of the mean of the squared amplitude
      double mean=0;
      double max =0;
      double power = 0;
      for(int y = 0 ; y < haar_size ; y++)
	{
	  float v = filtered[y];
	  mean += v;
	  power += v*v;
	  if (v>max)
	    max = v;
	}
      mean/=haar_size;
      power = sqrt(power/haar_size);
      // printf("power of bank %d is %g\n",filter,power);
      
      // find the std dev of this bank entry
      double dev = 0;
      int devcnt = 0;

      bank_energy[filter] = 0;
      for(int row = 0 ; row < haar_size ; row++)
	{
	  float v = filtered[row];
	  bank_energy[filter]+=v;
	  dev+=fabs(v-mean);
	  devcnt++;
	}
      bank_energy[filter]*=window_size;
      dev/=devcnt;

      // printf("Slice %d has mean energy of %g and deviation of %g and max of %g\n",filter,mean,dev,max);
      if (dev>0)
	for(int y = 0 ; y < haar_size ; y++)
	  {
	    float v = filtered[y];
	    v-=mean;
	    v/=dev;
	    v+=0.5;
	    if (v>1) v=1;
	    if (v<0) v=0;
	    filtered[y]=v;
	  }
    }
  
  // normalize bank_energy
  double maxe=0;
  for(int i = 0 ; i <= maxslice ; i++)
    if (bank_energy[i]>maxe)
      maxe=bank_energy[i];
  if (maxe>0)
    for(int i = 0 ; i <= maxslice ; i++)
      bank_energy[i]/=maxe;
  //  for(int i = 0 ; i <= maxslice ; i++)
  //    printf("bank entry %d has %g energy\n",i,bank_energy[i]);
}

void BeatGraphAnalyzer::showHaarPattern()
{ 
  unsigned4 collapsed_period = period  / COLLAPSE ;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  // show
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      int co = column * collapsed_period;
      for(int row = 0 ; row < window_ysize ; row++)
	{
	  QColor c;
	  int ro = co + (int)((float)row*yscale);
	  int r = 0, g = 0, b = 0;
	  for(int slice = maxslice; slice>=0 ; slice --)
	    {
	      int x1 = ro >> slice;
	      int x2 = (ro+1) >> slice;
	      float value = 0;
	      if (x2>x1)
		for(int x = x1 ; x < x2 ; x ++)
		  value += bank[slice][x] / (float)(x2 - x1);
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
  pattern->setPixmap(*pm);
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  l.end();
}

/**
 * Checks whether a descent visualisation is possible
 */
bool BeatGraphAnalyzer::check_visualisation_conditions()
{
  if (!period)
    {
      QMessageBox::warning(this,"No period estimate",
			   "No period estimate, hence cannot show the beat graph.\n"
			   "Please go to the bpm counter and measure the tempo first");
      return false;
    }
  if (!audiosize)
    {
      QMessageBox::warning(this,"Fragment too small",
			   "There is simply no raw data on disk,\n"
			   "Hence, I can't display the beat graph");
      return false;
    }
  return true;
}

/**
 * This function will update the pattern picture shown.
 */
void BeatGraphAnalyzer::showPattern()
{
  if (!check_visualisation_conditions()) return;
  if (haar->isChecked())
    showHaarPattern();
  else
    showEnergyPattern();
}


/**
 * This function must be called when the visualisation modus changes
 */
void BeatGraphAnalyzer::changeVisualisation()
{
  readFileSigned(); 
  if (!check_visualisation_conditions()) return;
  if (haar->isChecked())
    calculateHaar();
  else
    calculateEnergy();
  showPattern();
}


void BeatGraphAnalyzer::getTempo()
{
  period=normalperiod+periodDelta->value()+periodDelta10->value();
  if (period<=0) period=1;
}

void BeatGraphAnalyzer::slantChanged()
{
  getTempo();
  showPattern();
}

void BeatGraphAnalyzer::setTempo()
{
  normalperiod = normalperiod+periodDelta->value()+periodDelta10->value();
  if (normalperiod<=0) normalperiod = 1;
  ::y = ::x * currentperiod / normalperiod;
  periodDelta->setValue(0);
  periodDelta10->setValue(0);
  playing->set_period(normalperiod/4);
}
