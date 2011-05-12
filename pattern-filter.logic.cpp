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

#include "pattern-analyzer.logic.h"

#ifdef FILTER
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
#include "spectrumanalyzer.logic.h"
#include "sys/times.h"
#include "kbpm-play.h"
#include "fourier.h"
#include "version.h"
#include "scripts.h"

#define COLLAPSE 4
#define COLLAPSE_HAAR 4
#define sample_freq ((double)WAVRATE/(double)COLLAPSE)

void PatternAnalyzerLogic::filter_init()
{
  filter = NULL;
  filtersize = 0;
}

double PatternAnalyzerLogic::getGain(double f)
{
  if (f<0) return getGain(-f);
  if (f >= lo && f <= hi) return 1.0;
  else return 0.0;
}

double PatternAnalyzerLogic::inverseDtft(double n, double resolution)
{
  double x=0,y=0;
  for (double w = -M_PI ; w <= M_PI ; w += M_PI / resolution )
    {
      double g = getGain(sample_freq*w/(2.0*M_PI));
      x+=g*cos(w*n);
      y+=g*sin(w*n);
    }
  return (x+y)/(M_PI);
}

double * PatternAnalyzerLogic::doubleFilter(double * coef, int& nr)
{
  double *newcoef = allocate(nr*2,double);
  for(int i = 0 ; i < 2*nr; i++)
    newcoef[i]=0;
  for(int i = 0 ; i < nr ; i ++)
    for(int j = 0 ; j < nr; j ++)
      newcoef[i+j]+=coef[i]*coef[j];
  nr*=2;
  deallocate(coef);
  return newcoef;
}

void PatternAnalyzerLogic::createFilter()
{
  // eerst creeeren we een filter met de grootte filtersize/stacksize;
  // deze delen we door twee omdat de filter in 2 richtingen gedefinieerd wordt;
  int i, nr = filtersize / (2 * stacking->value());
  assert(nr);
  double * coef = allocate(nr*2,double);
  for(i = - nr + 1 ; i < nr; i++)
    coef [ i + nr - 1 ] = inverseDtft(i, nr) * 2.0 / (float)nr;
  nr*=2;
  nr--;
  // now double the filter stacksize - 1 times
  i = stacking->value();
  while( i-- > 1 ) coef = doubleFilter(coef,nr);
  deallocate(filter);
  filter = coef;
  filtersize = nr;
  // find the maximum gain within the pass band and divide all coefficients
  // by it..
  double max=0;
  for(double freq = lo ; freq<hi; freq += (hi-lo)/10.0 )
    {
      double g = g_frequency_response(freq);
      if (g>max) max=g;
    }
  if (max>0)
    for(int i = 0 ; i < filtersize; i ++)
      filter[i]/=max;
}

//---------------------------------------------------------------------
//
//  filter display
//
//---------------------------------------------------------------------
const double db_top = 3;
const double db_bottom = -140;

// WVB -- according to me the dynamic range of 16 bits is only 45 dB.
//        to exaplin this consider the maximu msignal amplitude 32768 (2^15)
//        to convert this to dB we take 10*log_10(32768)= 45.15 dB
//        therefore we simply cut the graphic at -50 dB
void PatternAnalyzerLogic::showFilter()
{
  int window_xsize = filterImage->contentsRect().width();
  int window_ysize = filterImage->contentsRect().height();
  assert(window_ysize > 0 && window_xsize > 0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  p.fillRect(QRect(0,0,window_xsize,window_ysize),Qt::white);
  double yscale = (db_bottom-db_top)/(float)window_ysize;
  // wegkleuren van de zones die niet gefilterd kunnen worden. 
  // dit is vanaf sample_freq/2 tot en met the end...
  double nyquist = sample_freq/2.0;
  for(int i = 0 ; i < barksize ; i ++)
    {
      if (barkbounds[i]>nyquist)
	{
	  i--;
	  double lower = barkbounds[i];
	  double higher = i > barksize ? 22050 : barkbounds[i+1];
	  float  posa = (i * window_xsize / (barksize + 1));
	  float  posb = ((i+1) * window_xsize/(barksize+1));
	  float  pos = posa + (nyquist - lower) * (posb - posa) / (higher - lower);
	  p.fillRect(QRect((int)pos,0,window_xsize-1,window_ysize-1),QColor(192,192,192));
	  break;
	}
    }
  // uitzetten van de 24 barklines
  p.setPen(QColor(128,128,128));
  for(int i = 0 ; i <= barksize ; i ++)
    {
      int pos = i * window_xsize/(barksize+1);
      p.drawLine(pos,0,pos,window_ysize-1);
      p.drawText(pos,window_ysize-1,QString::number(i));
    }
  // uitzetten van de dB lines op 0, 20, 40, 60, 80, 100, 120 & 140
  for(float i = 0 ; i > db_bottom ; i-=20)
    {
      float row = i/yscale;
      p.drawLine(0,(int)row,window_xsize-1,(int)row);
      p.drawText(0,(int)row,QString::number(i));
    }
  float row= -45.1/yscale;
  p.setPen(QColor(0,255,0));
  p.drawLine(0,(int)row,window_xsize-1,(int)row);
  p.setPen(QColor(255,0,0));
  float oco=0,oro=window_ysize-1;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      // eerst uitvissen in welk segment we zitten. 
      int segment = column * (barksize + 1) / window_xsize;
      assert(segment>=0 && segment <= barksize+1);
      double lower = barkbounds[segment];
      double higher = segment > barksize ? 22050 : barkbounds[segment+1];
      float  posa = (segment * window_xsize / (barksize + 1));
      float  posb = ((segment+1) * window_xsize/(barksize+1));
      float  freq = lower + (column-posa) * (higher-lower) / (posb-posa);
      // float freq = (float)column*sample_freq/(2.0*(float)window_xsize);
      float resp = db_frequency_response(freq,db_bottom);
      float row = (resp-db_top)/yscale;
      p.drawLine((int)oco,(int)oro,column,(int)row);
      oco=column;
      oro=row;
      // p.drawPoint(column,(int)row);
    }
  
  p.end(); 
  filterImage->setPixmap(*pm);
}

double PatternAnalyzerLogic::g_frequency_response(double freq)
{
  double xs=0;
  double ys=0;
  for(int i=0;i<filtersize;i++)
    {
      xs+=filter[i] * cos( freq * M_PI *2.0 * (double)i / sample_freq );
      ys+=filter[i] * sin( freq * M_PI *2.0 * (double)i / sample_freq );
    }
  return sqrt(xs*xs+ys*ys);
}

double PatternAnalyzerLogic::db_frequency_response(double freq, double clip)
{
  double result = g_frequency_response(freq);
  result = (log(result)/log(10))*10;
  if (result<clip) result=clip;
  return result;
}

void PatternAnalyzerLogic::filterChanged()
{
  // fetch values 
  bool change = false;
  lo = atod(locut->currentText());
  hi = atod(hicut->currentText());
  if (lo<0) lo = 0;
  if (hi>WAVRATE/2) hi = WAVRATE / 2;
  if (lo>=hi) lo = 0; change = true; 
  if (hi<=lo) hi = WAVRATE / 2;
  filtersize = atoi(order->text());
  // build new filter and show image...
  createFilter();
  showFilter();
  // release data so it will be read later on..
  deallocate(data);
  data=NULL;
  audiosize=0;
}

#else
void PatternAnalyzerLogic::showFilter() 
{
}

#endif
