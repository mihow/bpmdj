/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
 See 'BeatMixing.ps' for more information

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
#include <qmessagebox.h>
#include "pattern-analyzer.logic.h"
#include "spectrumanalyzer.logic.h"
#include "sys/times.h"
#include "kbpm-play.h"
#include "fourier.h"
#include "version.h"
#include "scripts.h"

#define COLLAPSE 4
#define COLLAPSE_HAAR 4
#define sample_freq ((double)WAVRATE/(double)COLLAPSE)

PatternAnalyzerLogic::PatternAnalyzerLogic(bool showreaderprogress, SongPlayer*parent, const char*name, bool modal, WFlags f) :
  PatternDialog(0,name,modal,f)
{
  period=normalperiod;
  data = NULL;
  signed_data = NULL;
  readFile(showreaderprogress);
  show();
  showPattern();
}

void PatternAnalyzerLogic::readFile(bool showreaderprogress)
{
  if (data) return;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  // read in memory and shrink it 
  FILE * raw=openRawFile(playing,arg_rawpath);
  audiosize=fsize(raw)/4;
  if (audiosize == 0) return;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  data = allocate(audiosize/COLLAPSE,compressed);
  
  QProgressDialog * progress = NULL;
  if (showreaderprogress) 
    {
      progress = new QProgressDialog();
      progress->setTotalSteps(audiosize/bufsize);
      progress->show();
    }

  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      if (progress) progress->setProgress(pos/bufsize);
      app->processEvents();

      for(int i = 0 ; i<count/COLLAPSE; i++)
	{
	  unsigned4 sum=0;
	  for(int j = 0 ; j < COLLAPSE ; j++)
	    {
	      assert(i*COLLAPSE+j<count);
	      sum+=abs(buffer[i*COLLAPSE+j].leftright.left)/(1<<7);
	      sum+=abs(buffer[i*COLLAPSE+j].leftright.right)/(1<<7);
	    }
	  sum /= COLLAPSE*2;
	  assert(i + pos/COLLAPSE < audiosize/COLLAPSE);
	  data[i+pos/COLLAPSE]=sum;;
	}
      pos+=count;
    }
  
  fclose(raw);
  delete(progress);
  
  // normalize the audio
  unsigned4 maximum = 0;
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    if (data[i]>maximum)
      maximum=data[i];
  if (maximum==0)
    {
      audiosize=0;
      return;
    }
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    data[i]=(signed8)data[i]*(signed8)255/maximum;
}

void PatternAnalyzerLogic::readFileSigned(bool showreaderprogress)
{
  if (data) 
    {
      deallocate(data);
      data = NULL;
    }
  if (signed_data)
    return;
  long int bufsize = 65536;
  longtrick buffer[bufsize];

  // read in memory and shrink it 
  FILE * raw=openRawFile(playing,arg_rawpath);
  audiosize=fsize(raw)/4;
  if (audiosize == 0) return;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  signed_data = allocate(audiosize/COLLAPSE_HAAR,uncompressed);
  
  QProgressDialog * progress = NULL;
  if (showreaderprogress) 
    {
      progress = new QProgressDialog();
      progress->setTotalSteps(audiosize/bufsize);
      progress->show();
    }
  
  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      if (progress) progress->setProgress(pos/bufsize);
      app->processEvents();

      for(int i = 0 ; i < count/COLLAPSE_HAAR; i++)
	{
	  uncompressed sum=0;
	  for(int j = 0 ; j < COLLAPSE_HAAR ; j++)
	    {
	      assert(i*COLLAPSE_HAAR+j<count);
	      sum+=buffer[i*COLLAPSE_HAAR+j].leftright.left;
	      sum+=buffer[i*COLLAPSE_HAAR+j].leftright.right;
	    }
	  sum /= COLLAPSE_HAAR*2;
	  assert(i + pos/COLLAPSE_HAAR < audiosize/COLLAPSE_HAAR);
	  signed_data[i+pos/COLLAPSE_HAAR]=sum;
	}
      pos+=count;
    }
  
  fclose(raw);
  delete(progress);
  
  // normalize the audio
  uncompressed maximum = 0;
  for(int i = 0 ; i < audiosize/COLLAPSE_HAAR ; i++)
    if (signed_data[i]>maximum)
      maximum=signed_data[i];
  if (maximum==0)
    {
      audiosize=0;
      return;
    }
  for(int i = 0 ; i < audiosize/COLLAPSE_HAAR ; i++)
    signed_data[i]/=maximum;
}


void PatternAnalyzerLogic::showPatternVersion1()
{ 
  int beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period  / COLLAPSE ;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  int dc = color->value();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  array(project, window_ysize, signed4);
  for(int row =  0 ; row < window_ysize ; row ++)
    project[row]=0;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      if (::x/(normalperiod*beats_per_column/4) == column)
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,0,0));
	    p.drawPoint(column,row);
	  }
      else
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,val,val));
	    p.drawPoint(column,row);
	  }
    }
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setPixmap(*pm);
  signed4 maximum=0;
  for(int row = 0 ; row < window_ysize ; row ++)
    if (project[row]>maximum)
      maximum=project[row];
  for(int row = 0 ; row < window_ysize ; row ++)
    project[row]=((signed8)project[row])*(signed8)255/(signed8)maximum;
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  for(int row = 0 ; row < window_ysize ; row++)
    {
      int val;
      val = project[row];
      l.setPen(QColor(val,val,val));
      l.drawPoint(0,row);
    }
  l.end();
  projection->setPixmap(*sm);
}


void PatternAnalyzerLogic::showHaarVersion1()
{ 
  unsigned4 collapsed_size = audiosize / COLLAPSE_HAAR ;
  
  int window_xsize = pattern->contentsRect().height();
  int window_ysize = pattern->contentsRect().height();
  assert(window_ysize>0);
  assert(window_xsize>0);
  
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  
  array(means, window_xsize, signed8*);
  for(int x = 0 ; x < window_xsize ; x ++)
    {
      means[x]=allocate(window_ysize,signed8);
      assert(means[x]);
      for(int y = 0 ; y < window_ysize ; y++)
	means[x][y]=0;
    }
  
  int fragment_size = collapsed_size / window_xsize;
  for(int y = 0 ; y <  window_ysize ; y ++)
    {
      // calculate windowsize and position
      // if y = 1 then we take the entire fragment_size log_2 = 0
      // if y = 2,3 we take half of it                  trunc(log_2) = 1
      // if y = 4,5,6,7 we take a fourth                trunc(loh
      int window_size = fragment_size;
      if (y>0)
	{
	  for(int p = y + 1 ; p>1 ; )
	    {
	      window_size/=2;
	      p/=2;
	    }
	  if (window_size<1) window_size = 1;
	}
      int window_position = y + 1 -  fragment_size / window_size ;
      printf("row %d, windowsize = %d, position %d\n",y,window_size, window_position);
      // calculate entire row
      for(int x = 0 ; x < window_xsize ; x ++)
	{
	  signed8 sum = 0;
	  for(int i = 0 ; i < window_size ; i++)
	    sum+=data[x*fragment_size+window_position*window_size+i];
	  means[x][y]=sum/window_size-means[x][y/2];
	}
      // find maximum uitslag
      signed8 max_amplitude = 0;
      for(int x = 0 ; x < window_xsize ; x ++)
	if (labs(means[x][y])>max_amplitude)
	  max_amplitude = labs(means[x][y]);
      // draw row
      for(int x = 0 ; x < window_xsize ; x ++)
	{
	  int val = labs(means[x][y])*255/max_amplitude;
	  p.setPen(QColor(val,val,val));
	  p.drawPoint(x,y);
	}
      // modify scale for next step
    }
  p.end(); 
  pattern->setPixmap(*pm);
}

const int maxslice = 8;
// maxslice 10 with HAAR_COLLAPSE 4: blokes van 1024 samples, samplerate van 11025, measured frequency = 10 Hz
// maxslice 8  with HAAR_COLLAPSE 4: blokes van 256 samples, samplerate van 11025, measured frequency = 40 Hz

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
      //assert(r<256);
      //assert(g<256);
      //assert(b<256);
      //printf("color = %d,%d,%d\n",r,g,b);
      color.setRgb(r,g,b);
    }
}

void PatternAnalyzerLogic::showHaarVersion2()
{ 
  // we should have sopme technique to reduce the smoothness of things. We want to increase
  // the contrast level of most signals...
  int beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period  / COLLAPSE_HAAR ;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4;
  unsigned4 collapsed_size = audiosize / COLLAPSE_HAAR ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  int dc = color->value();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  // first we calculate a number of layers, based on the audio-stream...
  // in every step we will modify the signed_data set by subtracting the current mean...
  array(bank,maxslice+1,float*);
  array(bank_energy,maxslice+1,double);
  bool th = treshold->isChecked();
  bool th2 = treshold2->isChecked();
  for(int filter = maxslice ; filter >= 0 ; filter --)
    {
      // calculate content of entry 'filter' of filterbank 'bank'
      int window_size = 1 << filter;
      int haar_size = collapsed_size / window_size;
      float * filtered = NULL;
      if (!filter)
	filtered = bank[filter] = signed_data;
      else
	{
	  filtered = bank[filter] = allocate(haar_size+1, float);
	  filtered[haar_size]=0;
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
	}

      for(int y = 0 ; y < haar_size ; y++)
	filtered[y]=fabs(filtered[y]);
      
      if (logarithmic->isChecked())
	{
	  float l2d = log(2);
	  for(int y = 0 ; y < haar_size ; y++)
	    {
	      float r = 10.0*log(filtered[y])/l2d;
	      if (r<-120) r=-120;
	      filtered[y]=(r+120.0)/120.0;
	    }
	}

      // smooth entry ?
      /*for(int y = 0 ; y < haar_size - 1; y++)
		{
	  float sum = 0;
	  for(int x = 0 ; x < 1 ; x++)
	    sum+=filtered[y+x];
	  filtered[y]=sum;
	}
      */
      
      if (decay->isChecked())
	{
	  float decay = 0.95;
	  float last = 0;
	  //for(int y = 0 ; y < haar_size; y++)
	  //last = (filtered[y] += decay * last);
	  for(int y = 0 ; y < haar_size; y++)
	    last = filtered[y] += 1 - exp(-last);
	}
      
      if (smear->isChecked())
	{
	  int ml = 2*(maxslice-filter);
	  if (ml>0)
	    for(int y = haar_size ; y > ml; y--)
	      {
		float sum = 0;
		for(int x = 0 ; x < ml ; x++)
		  {
		    float v = filtered[y-x];
		    if (v>sum)
		      sum=v;
		  }
		filtered[y]=sum;
	      }
	}
      
      // find the mean of this bank entry
      double mean=0;
      double max =0;
      for(int y = 0 ; y < haar_size ; y++)
	{
	  float v = filtered[y];
	  mean += v;
	  if (v>max)
	    max = v;
	}
      mean/=haar_size;
      
      if (th2)
	{
	  float mean2=0;
	  int m2c=0;
	  for(int y = 0 ; y < haar_size ; y++)
	    {
	      float v = filtered[y];
	      if (v>mean)
		{
		  mean2+=v;
		  m2c++;
		}
	    }
	  mean2/=m2c;
	  mean=mean2;
	}

      // find the std dev of this bank entry
      double dev = 0;
      int devcnt = 0;

      bank_energy[filter] = 0;
      for(int row = 0 ; row < haar_size ; row++)
	{
	  float v = filtered[row];
	  if (th)
	    {
	      if (v>mean)
		{
		  bank_energy[filter]+=v;
		  dev+=fabs(v-mean);
		  devcnt++;
		}
	    }
	  else
	    {
	      bank_energy[filter]+=v;
	      dev+=fabs(v-mean);
	      devcnt++;
	    }
	}
      bank_energy[filter]*=window_size;
      dev/=devcnt;

      printf("Slice %d has mean energy of %g and deviation of %g and max of %g\n",filter,mean,dev,max);
      if (dev>0)
	for(int y = 0 ; y < haar_size ; y++)
	  {
	    float v = filtered[y];
	    if (th && v<mean) v=0;
	    else
	      {
		v-=mean;
		v/=dev;
		v+=0.5;
		if (th2) v+=0.5;
		if (v>1) v=1;
		if (v<0) v=0;
	      }
	    filtered[y]=v;
	  }
    }
  
  signed_data = NULL;

  // normalize bank_energy
  double maxe=0;
  for(int i = 0 ; i <= maxslice ; i++)
    if (bank_energy[i]>maxe)
      maxe=bank_energy[i];
  if (maxe>0)
    for(int i = 0 ; i <= maxslice ; i++)
      bank_energy[i]/=maxe;
  for(int i = 0 ; i <= maxslice ; i++)
    printf("bank entry %d has %g energy\n",i,bank_energy[i]);

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
	  //int dc = color->value();
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
	      // we _DELEN_ door de bankenerrgy. De redenering hier is dat na de normalisatie elke
	      // band even zichtbaar zou moeten zijn
	      // als een band dan weinig energie heeft wilt dat zeggen dat deze vooral in pieken
	      // werkt. Om deze dan zichtbaar te maken is het nodig deze te vergoten, hence te delen
	      // getBandColor(maxslice-slice,c,value/bank_energy[slice]);
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

  // show the alignation data for every slice
  // if (allign->isChecked())
  if(0)
    for(int column = 0 ; column < window_xsize ; column++)
      {
	int x1 = column * collapsed_period;
	int x2 = (column+1) * collapsed_period;
	QColor c;
	double shift = 0;
	for(int slice = maxslice; slice>=0 ; slice --)
	  {
	    double accu = 0;
	    double weight = 0;
	    int x3 = x1 >> slice;
	    int x4 = x2 >> slice;
	    int xl = x4 - x3;
	    int xm = (x4 + x3) / 2;
	    for(int x = x1 >> slice ; x < x2 >> slice ; x ++)
	      {
		double v = bank[slice][x];
		weight += v;
		accu += v*(xm-x);
	      }
	    //accu*=xl
	    accu/=weight;
	    accu/=xl;
	    printf("phase = %g\n",accu);
	    accu*=window_ysize;
	    accu/=2;
	    accu+=window_ysize/2;
	    shift+=accu;
	    // we _DELEN_ door de bankenerrgy. De redenering hier is dat na de normalisatie elke
	    // band even zichtbaar zou moeten zijn
	    // als een band dan weinig energie heeft wilt dat zeggen dat deze vooral in pieken
	    // werkt. Om deze dan zichtbaar te maken is het nodig deze te vergoten, hence te delen
	    getBandColor(maxslice-slice,c,1);
	    p.setPen(c);
	    int row = (int)accu;
	    p.drawPoint(column,row);
	    p.setPen(QColor(0,0,0));
	    p.drawPoint(column,row-1);
	    p.drawPoint(column,row+1);
	  }
	shift/=maxslice;
	p.setPen(QColor(255,255,255));
	p.drawPoint(column,(int)shift);
      }
  
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setPixmap(*pm);
  signed4 maximum=0;
  //  for(int row = 0 ; row < window_ysize ; row ++)
    //if (project[row]>maximum)
    //  maximum=project[row];
  //  for(int row = 0 ; row < window_ysize ; row ++)
  //    project[row]=((signed8)project[row])*(signed8)255/(signed8)maximum;
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  //  for(int row = 0 ; row < window_ysize ; row++)
  //    {
  //      int val;
  //      val = project[row];
  //      l.setPen(QColor(val,val,val));
  //      l.drawPoint(0,row);
  //    }
  l.end();
  //  projection->setPixmap(*sm);
}

void PatternAnalyzerLogic::showHaarVersion3()
{ 
  int beats_per_column = beats->value() ;
  const int maxslice = 8;
  unsigned4 collapsed_period = period  / COLLAPSE_HAAR ;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4;
  unsigned4 collapsed_size = audiosize / COLLAPSE_HAAR ;
  int window_xsize = collapsed_size * (maxslice+1) / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  int dc = color->value();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  // array(project, window_ysize, signed4);
  //for(int row =  0 ; row < window_ysize ; row ++)
  // project[row]=0;
  for(int column = 0 ; column < window_xsize / (maxslice + 1); column++)
    {
      array(slices,maxslice+1,float*);
      for(int slice = maxslice ; slice >= 0 ; slice --)
	{
	  int window_size = 1 << slice;
	  int haar_size = collapsed_period / window_size;
	  slices[slice] = allocate(haar_size+1, float);
	  slices[slice][haar_size]=0;
	  for(int row = 0 ; row < haar_size ; row ++)
	    {
	      slices[slice][row]=0;
	      int offset = column*collapsed_period + row * window_size;
	      for(int x = 0 ; x < window_size ; x++)
		slices[slice][row] += signed_data[x+offset];
	      slices[slice][row]/=(float)window_size;
	    }
	  if (slice<maxslice)
	    for(int lower_freq = slice + 1 ; lower_freq <= maxslice ; lower_freq++)
	      for(int row = 0 ;row < haar_size ; row++)
		slices[slice][row]-=slices[lower_freq][row>>(lower_freq-slice)];
	}
      // normalize mean values to range [0..1];
      for(int slice = maxslice ; slice >=0 ; slice --)
	{
	  int window_size = 1 << slice;
	  int haar_size = collapsed_period / window_size;
	  float sum=0;
	  float max=0;
	  for(int row = 0 ; row < haar_size ; row++)
	    {
	      slices[slice][row]=fabs(slices[slice][row]);
	      sum += slices[slice][row];
	      if (slices[slice][row]>max)
		max=slices[slice][row];
	    }
	  sum/=haar_size;
	  //	  printf("Slice %d has mean energy of %g and maximum of %g\n",slice,sum,max);
	  if (max>0)
	    {
	      float p = slice/maxslice;
	      p *= p;
	      p *= 2;
	      p += 0.5;
	      for(int row = 0 ; row < haar_size ; row++)
		slices[slice][row]= pow(slices[slice][row]/max,p);
	    }
	}
      // visualize content by selecting maximum energy within range

      for(int slice = maxslice; slice>=0 ; slice --)
	{
	  int window_size = 1 << slice;
	  int haar_size = collapsed_period / window_size;
	  for(int row = 0 ; row < window_ysize ; row++)
	    {
	      QColor c;
	      int r = 0, g = 0, b = 0;
	      int x1 = row * haar_size / window_ysize;
	      int x2 = (row+1) * haar_size / window_ysize;
	      float value = 0;
	      if (x2>x1)
		for(int x = x1 ; x < x2 ; x ++)
		  value += slices[slice][x] / (float)(x2 - x1);
	      else
		value = slices[slice][x1];
	      int val = (int)(value*255.0);
	      if (val>255)
		val = 255;
	      c.setHsv(240*(maxslice-slice)/maxslice,255,val);
	      p.setPen(c);
	      p.drawPoint(column*(maxslice+1)+slice,row);
	    }
	}
    }
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setPixmap(*pm);
  signed4 maximum=0;
  //  for(int row = 0 ; row < window_ysize ; row ++)
    //if (project[row]>maximum)
    //  maximum=project[row];
  //  for(int row = 0 ; row < window_ysize ; row ++)
  //    project[row]=((signed8)project[row])*(signed8)255/(signed8)maximum;
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  //  for(int row = 0 ; row < window_ysize ; row++)
  //    {
  //      int val;
  //      val = project[row];
  //      l.setPen(QColor(val,val,val));
  //      l.drawPoint(0,row);
  //    }
  l.end();
  //  projection->setPixmap(*sm);
}

void PatternAnalyzerLogic::showPattern()
{
  if (!period)
    {
      QMessageBox::warning(this,"No period estimate",
			   "No period estimate, hence cannot show the beat graph.\n"
			   "Please go to the bpm counter and measure the tempo first");
      return;
    }
  
  if (haar->isChecked())
    readFileSigned(false);
  else
    readFile(false);

  if (!audiosize)
    {
      QMessageBox::warning(this,"Fragment too small",
			   "There is simply no raw data on disk,\n"
			   "Hence, I can't display the beat graph");
      return;
    }

  if (haar->isChecked())
    showHaarVersion2();
  else
    showPatternVersion1();
}

void PatternAnalyzerLogic::showPatternVersion2()
{ 
  int beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period  / COLLAPSE ;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  int dc = color->value();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  array(project, window_ysize, signed4);
  for(int row =  0 ; row < window_ysize ; row ++)
    project[row]=0;
  unsigned4 last_red;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      if (::x/(normalperiod*beats_per_column/4) == column)
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,val,val));
	    last_red = val;
	    p.drawPoint(column,row);
	  }
      else
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    int yel = abs(val - last_red);
	    last_red = val;
	    p.setPen(QColor(val,yel,0));
	    p.drawPoint(column,row);
	  }
    }
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setPixmap(*pm);
  signed4 maximum=0;
  for(int row = 0 ; row < window_ysize ; row ++)
    if (project[row]>maximum)
      maximum=project[row];
  for(int row = 0 ; row < window_ysize ; row ++)
    project[row]=((signed8)project[row])*(signed8)255/(signed8)maximum;
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  for(int row = 0 ; row < window_ysize ; row++)
    {
      int val;
      val = project[row];
      l.setPen(QColor(val,val,val));
      l.drawPoint(0,row);
    }
  l.end();
  projection->setPixmap(*sm);
}

void PatternAnalyzerLogic::settogrey()
{
  if (haar->isChecked())
    haar->setChecked(false);
}
  
 void PatternAnalyzerLogic::slantChanged()
   {
     settogrey();
     period=normalperiod+periodDelta->value()+periodDelta10->value();
  if (period<=0) period=1;
  showPattern();
}

void PatternAnalyzerLogic::balanceChanged()
{
  settogrey();
  showPattern();
}

void PatternAnalyzerLogic::setTempo()
{
  normalperiod = normalperiod+periodDelta->value()+periodDelta10->value();
  if (normalperiod<=0) normalperiod = 1;
  ::y = ::x * currentperiod / normalperiod;
  periodDelta->setValue(0);
  periodDelta10->setValue(0);
  playing->set_period(normalperiod/4);
}
