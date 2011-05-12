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
#include "version.h"
#include "scripts.h"
#include "avltree.cpp"

TempoLineAnalyzerLogic::TempoLineAnalyzerLogic(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  TempoLineDialog(0,name,modal,f)
{
  long int bufsize = 65536;
  // read in memory and shrink it 
  FILE * raw=openRawFile(playing,arg_rawpath);
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
	  max=(int)content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = (int)content[x][y];
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
	  max=(int)content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = (int)content[x][y];
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
	  max=(int)content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = (int)content[x][y];
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
	  max=(int)content[x][y];
      for (int y = 0 ; y < Y; y ++)
	{
	  int c = (int)content[x][y];
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



static long long int last_mean = 0;
static longtrick * last_mean_req = 0;
static int calc_mean(longtrick * block, int size)
{
  if (block==last_mean_req+1)
    {
      last_mean -= last_mean_req[0].leftright.left;
      last_mean_req=block;
      last_mean+=last_mean_req[size-1].leftright.left;
    }
  else
    {
      last_mean = 0;
      last_mean_req= block;
      for (int  i = 0 ; i < size ; i ++)
	last_mean+=last_mean_req[i].leftright.left;
    }
  return last_mean/size;
}


void TempoLineAnalyzerLogic::showCorrelationMatrix1()
{ 
  // dit klinkt gewoon veel te weird
  // we nemen een bepaalde blokgrootte
  int block_size = 2; // dit is gebasseerd op de impulse response eerer gemeten
  // eerst zoeken we de blok met de meeste energie op
  long long int max_energy = 0;
  long long int max_energy_at = 0;
  int * means = allocate(audiosize,int);
  long int * energies = allocate(audiosize,long int);
  for(int x = 0 ; x < audiosize - block_size ; x ++)
    {
      long long int mean = calc_mean(data + x, block_size);
      long long int energy = 0;
      for (int i = 0 ; i < block_size ; i++)
	energy += labs(data[x+i].leftright.left-mean);
      energies[x]=energy;
      means[x]=mean;
      if (energy > max_energy)
	{
	  max_energy = energy;
	  max_energy_at = x;
	}
      if (x%(audiosize/100)==0)
	{
	  printf("%g percent done\n", (float)x*100.0/(float)audiosize);
	  fflush(stdout);
	}
    }
  // goed laten we nu deze block kopieren en normalizeren 
  long * block = allocate(block_size,long);
  int mean = calc_mean(data + max_energy_at, block_size);
  for (int i = 0 ; i < block_size ; i++)
    block[i]=data[max_energy_at+i].leftright.left-mean;

  printf("maximum energy at %ld with value of %ld\n",(long)max_energy_at, (long)max_energy);
  printf("This is at position %g in the file\n",(float)max_energy_at/(float)audiosize);
  // nu we deze informatie hebben kunnen we door de sample lopen en de presence of 
  // onze block in kaart brengen
  short int * content = allocate(audiosize,short);
  for(int x = 0 ; x < audiosize - block_size ; x++)
    {
      // 0. calculate the mean of the match-with block
      int mean = means[x];
      // 1. calculate the mean deviation by deviding
      long int energy = energies[x];
      if (energy == max_energy)
	printf("PING\n");
      if (x == max_energy_at)
	printf("PONG\n");
      if (energy!=0) 
	{
	  // 2. Bereken de energetische winst die hieruit voortkomt
	  int remaining=0;
	  for (int i = 0 ; i < block_size ; i ++)
	    {
	      long int old_val = data[x+i].leftright.left;
	      long int new_val = old_val-(long long)block[i]*(long long)energy/(long long)max_energy; 
	      remaining += labs(new_val-mean);
	    }
	  // 3. der is dus 1.0 - remaining/energy terug gevonden
	  int c=100-remaining*100/energy;
	  //	  if (c<0)
	  //content[x] = 0;
	  // else
	    content[x] = c;
	}
      else
	content[x]=0;
      if (x%(audiosize/10)==0)
	{
	  printf("%g percent done\n", (float)x*100.0/(float)audiosize);
	  // create pixmap
	  int X = 1024;
	  int Y = 202;
	  QPixmap *pm = new QPixmap(X,Y);
	  QPainter p;
	  p.begin(pm);
	  QRect r(QRect(0,0,pm->width(),pm->height()));
	  p.fillRect(r,Qt::white);
	  p.setPen(QColor(255,0,0));
	  for(int i = 0 ; i < x ; i ++)
	    {
	      int px = (long long)i*(long long)X/(long long)x;
	      // if (content[i]>10)
		{
		  int py = content[i]*2;
		  p.drawPoint(px,py);
		}
	    }
	  p.end();
	  tempoline->setPixmap(*pm);
	  app->processEvents();
	}
    }
  // the next step is to find the largest value of content en fix this in the datastream
  // until no more content values are found
  // overal waar we een beetje winst maken zullen we deze nemen
  for (int maximum_winst = 100 ; maximum_winst > 0 ; maximum_winst --)
    {
      // find all occurences and fix audiostream
      for (int x = 0 ; x < audiosize - block_size ; x ++)
	if (content[x]==maximum_winst)
	  for (int i = 0 ; i < block_size ; i ++)
	    {
	      content[x+i]=-1;
	      content[x-i]=-1;
	      data[x+i].leftright.left -= (long long)block[i]*(long long)energies[x]/(long long)max_energy; 
	    }
      // update graphics
      {
	int X = 1024;
	int Y = 202;
	QPixmap *pm = new QPixmap(X,Y);
	QPainter p;
	p.begin(pm);
	QRect r(QRect(0,0,pm->width(),pm->height()));
	p.fillRect(r,Qt::white);
	p.setPen(QColor(255,0,0));
	for(int i = 0 ; i < audiosize ; i ++)
	  {
	    int px = (long long)i*(long long)X/(long long)audiosize;
	    if (content[i]>0)
	      {
		int py = content[i]*2;
		p.drawPoint(px,py);
	      }
	  }
	p.end();
	tempoline->setPixmap(*pm);
	app->processEvents();
      }
    }
  FILE * output = fopen("testje.raw","wb");
  assert(output);
  long written = 0;
  while(written<audiosize)
    if (audiosize-written > 65536)
      written += writesamples(data+written,65536,output);
    else
      written += writesamples(data+written,audiosize-written,output);
  fclose(output);
}



class AudioBlock
{
public:
  int * data;
  int size;
  int strength;
  AudioBlock(longtrick * pos, int s, int direction);
  bool isTheSame(AudioBlock * other);
};

bool AudioBlock::isTheSame(AudioBlock * other)
{
  long int sum = 0;
  for (int i = 0 ; i < size && sum < size * 4; i ++)
    sum += labs(data[i] - other->data[i]);
  // printf("sum = %d\n",sum);
  if (sum>size*4) return false;
  // printf("MATCH\n");
  return true;
}

AudioBlock::AudioBlock(longtrick * pos, int s, int direction)
{
  size = s;
  if (!size) return;
  data = allocate(size, int);
  int sign = 0;
  int max = 0;
  // printf("--------------------\n");
  for (int i = 0 ; i < s ; i ++)
    {
      int d = pos[i].leftright.left;
      d*=direction;
      if (d>max) max = d;
      data[i] = d;
      // printf("   |   %d \n",data[i]);
      // fflush(stdout);
      /*if (sign == 0)
	{
	  if (data[i]>0)
	    sign = 1;
	  else if (data[i]==0)
	    sign = 0;
	  else if (data[i]<0)
	    sign = -1;
	}
      else if (sign < 0)
	assert (data[i]<=0);
      else if (sign > 0)
      assert (data[i]>=0); */
      assert(d>=0);
    }
  /*  if (!max) max = 1;
  for (int i = 0 ; i < s ; i ++)
    {
      data[i]*=32768;
      data[i]/=max;
      }*/
  strength = direction;
}

class AudioFragmentNode: public Node<int>
{
private:
  int count;
  AudioBlock ** blocks;
  int container_size;
public:
  int size;
  AudioFragmentNode(int s);
  virtual int getKey();
  virtual int compareAddData(Node<int>* other);
  virtual int compareSearchData(int key);
  virtual void print();
  void addBlock(AudioBlock * block);
  int removeDuplicates();
};

int AudioFragmentNode::removeDuplicates()
{
  int duplicates = 0;
  for(int i = 0 ; i < count ; i ++)
    if (blocks[i])
      for(int j = i+1 ; j < count ; j ++)
	if (blocks[j] && blocks[i]->isTheSame(blocks[j]))
	  {
	    blocks[j]=NULL;
	    duplicates++;
	  }
  return duplicates;
}

void AudioFragmentNode::addBlock(AudioBlock * block)
{
  assert(block);
  assert(block->size == size);
  count ++;
  if (count>=container_size)
    blocks=reallocate(blocks,container_size*=2,AudioBlock*);
  blocks[count-1]=block;
}

AudioFragmentNode::AudioFragmentNode(int s)
{
  size = s;
  count = 0;
  blocks = allocate(1,AudioBlock*);
  container_size = 1;
}

int AudioFragmentNode::getKey() 
{
  return size; 
};

int AudioFragmentNode::compareAddData(Node<int>* other)
{
  return compareSearchData(((AudioFragmentNode*)other)->size);
}
  
int AudioFragmentNode::compareSearchData(int key)
{
  if (size > key ) 
    return -1;
  else if (size == key)
    return 0;
  else
    return 1;
}

void AudioFragmentNode::print()
{
  printf("size : %d, count : %d ",size,count);
  fflush(stdout);
}

void TempoLineAnalyzerLogic::showCorrelationMatrix2()
{ 
  // 0. first we count the number of blocks
  int lookingfor = 1; // 1 is positive, -1 is negative
  int breaks = -1;
  long int lastbreak = 0;
  AvlTree<int> *blocks = new AvlTree<int>();
  for(int x = 0 ; x < audiosize ; x++)
    {
      int d = data[x].leftright.left;
      if ( d == 0 ) continue;
      if ( ( d < 0 && lookingfor == -1 )
	|| ( d > 0 && lookingfor == 1  ) )
	{
	  // de start ligt op de laatste positie
	  int size = x - lastbreak;
	  AudioFragmentNode * frag = (AudioFragmentNode*)blocks->search(size);
	  if (!frag)
	    {
	      frag = new AudioFragmentNode(size);
	      blocks->add(frag);
	    }
	  lookingfor = - lookingfor;
	  frag->addBlock(new AudioBlock(data+lastbreak,size, lookingfor));
	  lastbreak = x;
	  breaks ++;
	}
    }
  printf(" %d breaks in %d data. This means a mean between break size of %g\n",
	 breaks, audiosize, (float)audiosize/(float)breaks);
  blocks->print();
  // 1. Find duplicates in the audioblocks...
  AudioFragmentNode * fragment;
  while (fragment = (AudioFragmentNode*)blocks->top())
    {
      fragment->print();
      printf(" %d\n",fragment->removeDuplicates());
      blocks->del(fragment->size);
    }
  delete blocks;
}

/*  long long int max_energy = 0;
  long long int max_energy_at = 0;
  int * means = allocate(audiosize,int);
  long int * energies = allocate(audiosize,long int);
  for(int x = 0 ; x < audiosize - block_size ; x ++)
    {
      long long int mean = calc_mean(data + x, block_size);
      long long int energy = 0;
      for (int i = 0 ; i < block_size ; i++)
	energy += labs(data[x+i].leftright.left-mean);
      energies[x]=energy;
      means[x]=mean;
      if (energy > max_energy)
	{
	  max_energy = energy;
	  max_energy_at = x;
	}
      if (x%(audiosize/100)==0)
	{
	  printf("%g percent done\n", (float)x*100.0/(float)audiosize);
	  fflush(stdout);
	}
    }
  // goed laten we nu deze block kopieren en normalizeren 
  long * block = allocate(block_size,long);
  int mean = calc_mean(data + max_energy_at, block_size);
  for (int i = 0 ; i < block_size ; i++)
    block[i]=data[max_energy_at+i].leftright.left-mean;

  printf("maximum energy at %ld with value of %ld\n",(long)max_energy_at, (long)max_energy);
  printf("This is at position %g in the file\n",(float)max_energy_at/(float)audiosize);
  // nu we deze informatie hebben kunnen we door de sample lopen en de presence of 
  // onze block in kaart brengen
  short int * content = allocate(audiosize,short);
  for(int x = 0 ; x < audiosize - block_size ; x++)
    {
      // 0. calculate the mean of the match-with block
      int mean = means[x];
      // 1. calculate the mean deviation by deviding
      long int energy = energies[x];
      if (energy == max_energy)
	printf("PING\n");
      if (x == max_energy_at)
	printf("PONG\n");
      if (energy!=0) 
	{
	  // 2. Bereken de energetische winst die hieruit voortkomt
	  int remaining=0;
	  for (int i = 0 ; i < block_size ; i ++)
	    {
	      long int old_val = data[x+i].leftright.left;
	      long int new_val = old_val-(long long)block[i]*(long long)energy/(long long)max_energy; 
	      remaining += labs(new_val-mean);
	    }
	  // 3. der is dus 1.0 - remaining/energy terug gevonden
	  int c=100-remaining*100/energy;
	  //	  if (c<0)
	  //content[x] = 0;
	  // else
	    content[x] = c;
	}
      else
	content[x]=0;
      if (x%(audiosize/10)==0)
	{
	  printf("%g percent done\n", (float)x*100.0/(float)audiosize);
	  // create pixmap
	  int X = 1024;
	  int Y = 202;
	  QPixmap *pm = new QPixmap(X,Y);
	  QPainter p;
	  p.begin(pm);
	  QRect r(QRect(0,0,pm->width(),pm->height()));
	  p.fillRect(r,Qt::white);
	  p.setPen(QColor(255,0,0));
	  for(int i = 0 ; i < x ; i ++)
	    {
	      int px = (long long)i*(long long)X/(long long)x;
	      // if (content[i]>10)
		{
		  int py = content[i]*2;
		  p.drawPoint(px,py);
		}
	    }
	  p.end();
	  tempoline->setPixmap(*pm);
	  app->processEvents();
	}
    }
  // the next step is to find the largest value of content en fix this in the datastream
  // until no more content values are found
  // overal waar we een beetje winst maken zullen we deze nemen
  for (int maximum_winst = 100 ; maximum_winst > 0 ; maximum_winst --)
    {
      // find all occurences and fix audiostream
      for (int x = 0 ; x < audiosize - block_size ; x ++)
	if (content[x]==maximum_winst)
	  for (int i = 0 ; i < block_size ; i ++)
	    {
	      content[x+i]=-1;
	      content[x-i]=-1;
	      data[x+i].leftright.left -= (long long)block[i]*(long long)energies[x]/(long long)max_energy; 
	    }
      // update graphics
      {
	int X = 1024;
	int Y = 202;
	QPixmap *pm = new QPixmap(X,Y);
	QPainter p;
	p.begin(pm);
	QRect r(QRect(0,0,pm->width(),pm->height()));
	p.fillRect(r,Qt::white);
	p.setPen(QColor(255,0,0));
	for(int i = 0 ; i < audiosize ; i ++)
	  {
	    int px = (long long)i*(long long)X/(long long)audiosize;
	    if (content[i]>0)
	      {
		int py = content[i]*2;
		p.drawPoint(px,py);
	      }
	  }
	p.end();
	tempoline->setPixmap(*pm);
	app->processEvents();
      }
    }
  FILE * output = fopen("testje.raw","wb");
  assert(output);
  long written = 0;
  while(written<audiosize)
    if (audiosize-written > 65536)
      written += writesamples(data+written,65536,output);
    else
      written += writesamples(data+written,audiosize-written,output);
  fclose(output);
}
*/


void TempoLineAnalyzerLogic::balanceChanged()
{
  showTempoLine();
}
