/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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
#include <libgen.h>
#include <qslider.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "tempolineanalyzer.logic.h"
#include "sys/times.h"
#include "fourier.h"
#include "kbpm-play.h"

extern "C"
{
#include "cbpm-index.h"
#include "version.h"
#include "scripts.h"
}

TempoLineAnalyzerLogic::TempoLineAnalyzerLogic(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  TempoLineDialog(0,name,modal,f)
{
  long int bufsize = 65536;
  // read in memory and shrink it 
  FILE * raw=openRawFile(arg_rawpath);
  audiosize=fsize(raw)/4;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  data = allocate(audiosize,longtrick);
  QProgressDialog * progress = new QProgressDialog();
  progress->setTotalSteps(audiosize);
  progress->show();
  while(pos<audiosize)
    {
      pos += readsamples((unsigned4*)data+pos,bufsize,raw);
      progress->setProgress(pos);
      app->processEvents();
    }
  fclose(raw);
  delete(progress);
  audiorate = 44100;
  //  showTempoLine();
}

void TempoLineAnalyzerLogic::showTempoLine_1()
{ 
  long int bpm_lo = 80;
  long int bpm_hi = 180;
  assert(bpm_lo<bpm_hi);
  // aan x beats/minute hebben we 
  // een periode per beat van 1/x minute/beat
  // dit is dus 4/x minute/measure
  // dit komt neer op 4*audiorate/x samples/measure
  long int window_lo = audiorate*4*60/bpm_hi;
  long int window_hi = audiorate*4*60/bpm_lo;
  assert(window_lo<window_hi);
  // we lopen doorheen de muziekbij stepsize
  // long int step_size = (window_lo+window_hi)/8;
      // long int steps = audiosize / step_size;
  // de windowsize pakken we gelijk aan de kortste lengte
  long int block_size = window_hi;
  long int window_size = window_hi-window_lo;
  // de maten van de window en colorbalance
  int X = tempoline->contentsRect().width();
  int Y = tempoline->contentsRect().height();
  // float array to normalize afterwards
  float **content = allocate(X,float*);
  for(int i = 0 ; i < X ; i ++)
    {
      content[i]=allocate(Y,float);
      for(int j = 0 ; j < Y ; j ++)
	content[i][j]=0.0;
    }
  
  // calculate content
  //for(int x = X / 3 ; x < X * 2 / 3; x ++)
  for(int x = 0 ; x < X ; x ++)
    {
      long long int rrx = (long long int)(audiosize - window_hi - block_size) * (long long int)x / (long long int)X;
      long int rx = rrx;
      assert(rx>=0);
      //printf("%d %d %ld\n",x,y,total);
      printf("== %g in file ========================\n",(float)rx/(float)audiosize);
      if (rx>=audiosize)
	{
	  printf("Skipping column %d\n",x);
	  break;
	}
      for (int y = 0 ; y < Y; y ++)
	{
	  rx = rrx;
	  long int ry = y * window_size / Y;
	  assert(ry>=0);
	  ry += window_lo;
	  //long int total = 0;
	  if (rx+ry>=audiosize)
	    {
	      printf("Skipping row %d\n",y);
	      break;
	    }
	  double devx=0;
	  double devy=0;
	  double covar=0;
	  for(int z = 0 ; z < block_size ; z++, rx ++)
	    {
	      if (rx+ry>=audiosize)
		{
		  printf("skipping slide\n");
		  break;
		}
	      double xi = data[rx].leftright.left;
	      double yi = data[rx+ry].leftright.left;
	      covar += xi*yi;
	      devx  += xi*xi;
	      devy  += yi*yi;
	      // long diff = -data[rx+ry].leftright.left;
	      //if (diff<0) total-=diff;
	      //else total+=diff;
	    }
	  devx/=(double)block_size;
	  devy/=(double)block_size;
	  covar/=(double)block_size;
	  devx=sqrt(devx);
	  devy=sqrt(devy);
	  double cor = covar/(devx*devy);
	  content[x][y] = cor;
	}
    }

  // find maximum and minimum values
  float maximum, minimum;
  minimum = maximum = content[0][0];
  for (int i = 0 ; i < X ; i++)
    for(int j = 0 ; j < Y ; j++)
      {
	float c = content[i][j];
	if (c<minimum) minimum = c;
	if (c>maximum) maximum = c;
      }
  printf("maximum = %g\nminimum = %g\n",maximum,minimum);
  for(int i = 0 ; i < X ; i ++)
    for(int j = 0 ; j < Y ; j++)
      {
	float c = content[i][j];
	c -= minimum;
	c /= maximum-minimum;
	c *= c;
	c *= 255.0;
	content[i][j]=c;
      }
  
  // rescale everything
  // for (int i = 0 ; i < X ; i++)
  
  // create pixmap
  QPixmap *pm = new QPixmap(X,Y);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::black);
  printf("window_lo = %ld\n window_hi = %ld\n",window_lo,window_hi);
  for(int x = 0 ; x < X ; x ++)
    {
      int max = 0;
      for (int y = 0 ; y < Y; y++)
	if (content[x][y]>max)
	  max=content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = content[x][y];
	  if (c==max)
	    p.setPen(QColor(0,c,0));
	  else
	    p.setPen(QColor(c,c,c));
	  p.drawPoint(x,y);
	}
    }
  p.end();
  tempoline->setPixmap(*pm);
}

void TempoLineAnalyzerLogic::showTempoLine_2()
{ 
  long int bpm_lo = 80;
  long int bpm_hi = 180;
  assert(bpm_lo<bpm_hi);
  // aan x beats/minute hebben we 
  // een periode per beat van 1/x minute/beat
  // dit is dus 4/x minute/measure
  // dit komt neer op 4*audiorate/x samples/measure
  long int window_lo = audiorate*4*60/bpm_hi;
  long int window_hi = audiorate*4*60/bpm_lo;
  assert(window_lo<window_hi);
  // we lopen doorheen de muziekbij stepsize
  // long int step_size = (window_lo+window_hi)/8;
      // long int steps = audiosize / step_size;
  // de windowsize pakken we gelijk aan de kortste lengte
  long int block_size = window_hi;
  long int window_size = window_hi-window_lo;
  // de maten van de window en colorbalance
  int X = tempoline->contentsRect().width();
  int Y = tempoline->contentsRect().height();
  // float array to normalize afterwards
  float **content = allocate(X,float*);
  for(int i = 0 ; i < X ; i ++)
    {
      content[i]=allocate(Y,float);
      for(int j = 0 ; j < Y ; j ++)
	content[i][j]=0.0;
    }
  
  // calculate content
  //for(int x = X / 3 ; x < X * 2 / 3; x ++)
  for(int x = 0 ; x < X ; x ++)
    {
      long long int rrx = (long long int)(audiosize - window_hi - block_size) * (long long int)x / (long long int)X;
      long int rx = rrx;
      assert(rx>=0);
      //printf("%d %d %ld\n",x,y,total);
      printf("== %g in file ========================\n",(float)rx/(float)audiosize);
      if (rx>=audiosize)
	{
	  printf("Skipping column %d\n",x);
	  break;
	}
      for (int y = 0 ; y < Y; y ++)
	{
	  rx = rrx;
	  long int ry = y * window_size / Y;
	  assert(ry>=0);
	  ry += window_lo;
	  //long int total = 0;
	  if (rx+ry>=audiosize)
	    {
	      printf("Skipping row %d\n",y);
	      break;
	    }
	  double devx=0;
	  double devy=0;
	  double covar=0;
	  for(int z = 0 ; z < block_size ; z++, rx ++)
	    {
	      if (rx+ry>=audiosize)
		{
		  printf("skipping slide\n");
		  break;
		}
	      double xi = data[rx].leftright.left;
	      double yi = data[rx+ry].leftright.left;
	      covar += xi*yi;
	      devx  += xi*xi;
	      devy  += yi*yi;
	      // long diff = -data[rx+ry].leftright.left;
	      //if (diff<0) total-=diff;
	      //else total+=diff;
	    }
	  devx/=(double)block_size;
	  devy/=(double)block_size;
	  covar/=(double)block_size;
	  devx=sqrt(devx);
	  devy=sqrt(devy);
	  double cor = covar/(devx*devy);
	  if (cor<0)
	    content[x][y] = -cor;
	  else
	    content[x][y] = cor;
	}
    }

  // find maximum and minimum values
  float maximum, minimum;
  for (int i = 0 ; i < X ; i++)
    {
      minimum = maximum = content[i][0];
      for(int j = 0 ; j < Y ; j++)
	{
	  float c = content[i][j];
	  if (c<minimum) minimum = c;
	  if (c>maximum) maximum = c;
	}
      printf("maximum = %g\nminimum = %g\n",maximum,minimum);
      for(int j = 0 ; j < Y ; j++)
	{
	  float c = content[i][j];
	  c -= minimum;
	  c /= maximum-minimum;
	  c *= c;
	  c *= 255.0;
	  content[i][j]=c;
	}
    }
  
  // rescale everything
  // for (int i = 0 ; i < X ; i++)
  
  // create pixmap
  QPixmap *pm = new QPixmap(X,Y);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::black);
  printf("window_lo = %ld\n window_hi = %ld\n",window_lo,window_hi);
  for(int x = 0 ; x < X ; x ++)
    {
      int max = 0;
      for (int y = 0 ; y < Y; y++)
	if (content[x][y]>max)
	  max=content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = content[x][y];
	  if (c==max)
	    p.setPen(QColor(0,c,0));
	  else
	    p.setPen(QColor(c,c,c));
	  p.drawPoint(x,y);
	}
    }
  p.end();
  tempoline->setPixmap(*pm);
}













void TempoLineAnalyzerLogic::showTempoLine_3()
{ 
  long int bpm_lo = 80;
  long int bpm_hi = 180;
  assert(bpm_lo<bpm_hi);
  // aan x beats/minute hebben we 
  // een periode per beat van 1/x minute/beat
  // dit is dus 4/x minute/measure
  // dit komt neer op 4*audiorate/x samples/measure
  long int window_lo = audiorate*4*60/bpm_hi;
  long int window_hi = audiorate*4*60/bpm_lo;
  assert(window_lo<window_hi);
  // we lopen doorheen de muziekbij stepsize
  // long int step_size = (window_lo+window_hi)/8;
      // long int steps = audiosize / step_size;
  // de windowsize pakken we gelijk aan de kortste lengte
  long int block_size = window_hi;
  long int window_size = window_hi-window_lo;
  // de maten van de window en colorbalance
  int X = tempoline->contentsRect().width();
  int Y = tempoline->contentsRect().height();
  // float array to normalize afterwards
  float **content = allocate(X,float*);
  for(int i = 0 ; i < X ; i ++)
    {
      content[i]=allocate(Y,float);
      for(int j = 0 ; j < Y ; j ++)
	content[i][j]=0.0;
    }
  
  // calculate content
  //for(int x = X / 3 ; x < X * 2 / 3; x ++)
  for(int x = 0 ; x < X ; x ++)
    {
      long long int rrx = (long long int)(audiosize - window_hi - block_size) * (long long int)x / (long long int)X;
      long int rx = rrx;
      assert(rx>=0);
      //printf("%d %d %ld\n",x,y,total);
      printf("== %g in file ========================\n",(float)rx/(float)audiosize);
      if (rx>=audiosize)
	{
	  printf("Skipping column %d\n",x);
	  break;
	}
      for (int y = 0 ; y < Y; y ++)
	{
	  rx = rrx;
	  long int ry = y * window_size / Y;
	  assert(ry>=0);
	  ry += window_lo;
	  //long int total = 0;
	  if (rx+ry>=audiosize)
	    {
	      printf("Skipping row %d\n",y);
	      break;
	    }
	  double total=0;
	  for(int z = 0 ; z < block_size ; z++, rx ++)
	    {
	      if (rx+ry>=audiosize)
		{
		  printf("skipping slide\n");
		  break;
		}
	      double xi = fabs(data[rx].leftright.left);
	      double yi = fabs(data[rx+ry].leftright.left);
	      total += fabs(xi-yi);
	    }
	  content[x][y] = total;
	}
    }
  
  // find maximum and minimum values
  float maximum, minimum;
  minimum = maximum = content[0][0];
  for (int i = 0 ; i < X ; i++)
    for(int j = 0 ; j < Y ; j++)
      {
	float c = content[i][j];
	if (c<minimum) minimum = c;
	if (c>maximum) maximum = c;
      }
  printf("maximum = %g\nminimum = %g\n",maximum,minimum);
  // rescale everything
  for (int i = 0 ; i < X ; i++)
    for(int j = 0 ; j < Y ; j++)
      {
	float c = content[i][j];
	c -= minimum;
	c /= maximum-minimum;
	c *= 255.0;
	content[i][j]=255.0-c;
      }
  
  // create pixmap
  QPixmap *pm = new QPixmap(X,Y);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::black);
  printf("window_lo = %ld\n window_hi = %ld\n",window_lo,window_hi);
  for(int x = 0 ; x < X ; x ++)
    {
      int max = 0;
      for (int y = 0 ; y < Y; y++)
	if (content[x][y]>max)
	  max=content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = content[x][y];
	  if (c==max)
	    p.setPen(QColor(0,c,0));
	  else
	    p.setPen(QColor(c,c,c));
	  p.drawPoint(x,y);
	}
    }
  p.end();
  tempoline->setPixmap(*pm);
}











void TempoLineAnalyzerLogic::showTempoLine_4()
{ 
  long int bpm_lo = 80;
  long int bpm_hi = 180;
  assert(bpm_lo<bpm_hi);
  // aan x beats/minute hebben we 
  // een periode per beat van 1/x minute/beat
  // dit is dus 4/x minute/measure
  // dit komt neer op 4*audiorate/x samples/measure
  long int window_lo = audiorate*4*60/bpm_hi;
  long int window_hi = audiorate*4*60/bpm_lo;
  assert(window_lo<window_hi);
  // we lopen doorheen de muziekbij stepsize
  // long int step_size = (window_lo+window_hi)/8;
      // long int steps = audiosize / step_size;
  // de windowsize pakken we gelijk aan de kortste lengte
  long int block_size = window_hi;
  long int window_size = window_hi-window_lo;
  // de maten van de window en colorbalance
  int X = tempoline->contentsRect().width();
  int Y = tempoline->contentsRect().height();
  // float array to normalize afterwards
  float **content = allocate(X,float*);
  for(int i = 0 ; i < X ; i ++)
    {
      content[i]=allocate(Y,float);
      for(int j = 0 ; j < Y ; j ++)
	content[i][j]=0.0;
    }
  
  // calculate content
  //for(int x = X / 3 ; x < X * 2 / 3; x ++)
  for(int x = 0 ; x < X ; x ++)
    {
      long long int rrx = (long long int)(audiosize - window_hi - block_size) * (long long int)x / (long long int)X;
      long int rx = rrx;
      assert(rx>=0);
      //printf("%d %d %ld\n",x,y,total);
      printf("== %g in file ========================\n",(float)rx/(float)audiosize);
      if (rx>=audiosize)
	{
	  printf("Skipping column %d\n",x);
	  break;
	}
      for (int y = 0 ; y < Y; y ++)
	{
	  rx = rrx;
	  long int ry = y * window_size / Y;
	  assert(ry>=0);
	  ry += window_lo;
	  //long int total = 0;
	  if (rx+ry>=audiosize)
	    {
	      printf("Skipping row %d\n",y);
	      break;
	    }
	  double total=0;
	  for(int z = 0 ; z < block_size ; z++, rx ++)
	    {
	      if (rx+ry>=audiosize)
		{
		  printf("skipping slide\n");
		  break;
		}
	      double xi = fabs(data[rx].leftright.left);
	      double yi = fabs(data[rx+ry].leftright.left);
	      double diff = xi-yi;
	      total += diff*diff; 
	    }
	  content[x][y] = total;
	}
    }
  
  
  // find maximum and minimum values
  float maximum, minimum;
  for (int i = 0 ; i < X ; i++)
    {
      minimum = maximum = content[i][0];
      for(int j = 0 ; j < Y ; j++)
	{
	  float c = content[i][j];
	  if (c<minimum) minimum = c;
	  if (c>maximum) maximum = c;
	}
      printf("maximum = %g\nminimum = %g\n",maximum,minimum);
      for(int j = 0 ; j < Y ; j++)
	{
	  float c = content[i][j];
	  c -= minimum;
	  c /= maximum-minimum;
	  c *= 255.0;
	  content[i][j]=255.0-c;
	}
    }
  
  // create pixmap
  QPixmap *pm = new QPixmap(X,Y);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::black);
  printf("window_lo = %ld\n window_hi = %ld\n",window_lo,window_hi);
  for(int x = 0 ; x < X ; x ++)
    {
      int max = 0;
      for (int y = 0 ; y < Y; y++)
	if (content[x][y]>max)
	  max=content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = content[x][y];
	  if (c==max)
	    p.setPen(QColor(0,c,0));
	  else
	    p.setPen(QColor(c,c,c));
	  p.drawPoint(x,y);
	}
    }
  p.end();
  tempoline->setPixmap(*pm);
}

void TempoLineAnalyzerLogic::balanceChanged()
{
  showTempoLine();
}
