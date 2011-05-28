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
#ifndef __loaded__statistics_cpp__
#define __loaded__statistics_cpp__
using namespace std;
#line 1 "statistics.c++"
#include <cstdio>
#include <assert.h>
#include <qstring.h>
#include <stdio.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qtextedit.h>
#include <qlabel.h>
#include "bpmdj.h"
#include "song.h"
#include "histogram-type.h"
#include "echo-property.h"
#include "selector.h"
#include "spectrum-type.h"
#include "signals.h"
#include "files.h"
#include "statistics.h"
#include "memory.h"
#include "pca.h"
#include "vector-iterator.h"
#include "database.h"

spectrum_freq devs[spectrum_size] = 
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
spectrum_freq means[spectrum_size] = 
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// unsigned8 total_music_body = 0;
static int spectra = 0;

float8 **echo_mean=NULL;
float8 **echo_dev=NULL;

static int echos = 0;

/**
 * In comparison to pre2.7 versions, the spectrum scale is now calculated
 * differently first we take the mean value. Based on this we also take the 
 * mean variation rescaling happened by subtracting the mean and then dividing
 * by the standard deviation
 */
void mean_spectrum(spectrum_type *spectrum)
{
  if (spectrum==no_spectrum) return;
  spectra++;
  for(int i = 0 ;i < spectrum_size; i ++ )
    means[i]+=spectrum->band(i);
}

void dev_spectrum(spectrum_type *spectrum)
{
  if (spectrum==no_spectrum) return;
  for(int i = 0 ;i < spectrum_size; i ++ )
    {
      float4 d = spectrum->band(i)-means[i];
      devs[i]+=d*d;
    }
}

void stop_mean_spectrum()
{
  if (spectra>0)
    for(int i = 0 ; i < spectrum_size ; i ++ )
      means[i]/=spectra;
}

void stop_dev_spectrum()
{
  if (spectra-1>0)
    for(int i = 0 ; i < spectrum_size ; i ++ )
      devs[i]=sqrt(devs[i]/(spectra-1));
}

float4 normalize_freq(float4 in, int band)
{
  in-=means[band];
  if (devs[band]>0)
    in/=devs[band];
  return in;
}

void mean_echo_old(echo_property echo)
{
  if (echo.empty()) return;
  echos++;
  if (echo_mean==NULL)
    {
      echo_mean=bpmdj_allocate(spectrum_size,float8*);
      echo_dev=bpmdj_allocate(spectrum_size,float8*);
      for(int i = 0 ; i < spectrum_size ; i ++)
	{
	  echo_mean[i]=bpmdj_allocate(echo_prop_sx,float8);
	  echo_dev[i]=bpmdj_allocate(echo_prop_sx,float8);
	  for(int j = 0 ; j < echo_prop_sx ; j++)
	    echo_mean[i][j]=echo_dev[i][j]=0;
	}
    }
  for(int i = 0 ;i < spectrum_size; i ++ )
    {
      int k = echo.get_count(i);
      for(int j = 0 ; j < k ; j ++)
	echo_mean[i][j]+=echo.get_freq_energy_probability(i,j);
    }
}

void stop_mean_echo_old()
{
  if (echos>0)
    for(int i = 0 ;i < spectrum_size; i ++ )
      for(int j = 0 ; j < echo_prop_sx ; j ++)
	echo_mean[i][j]/=echos;
}

void dev_echo(echo_property echo)
{
  if (echo.empty()) return;
  for(int i = 0 ;i < spectrum_size; i ++ )
    {
      int k = echo.get_count(i);
      for(int j = 0 ; j < k ; j ++)
	{
	  float4 d = echo.get_freq_energy_probability(i,j)-echo_mean[i][j];
	  echo_dev[i][j]+=d*d;
	}
    }
}

void stop_dev_echo()
{
  if (echos-1>0)
    for(int i = 0 ;i < spectrum_size; i ++ )
      for(int j = 0 ; j < echo_prop_sx ; j ++)
	echo_dev[i][j]=sqrt(echo_dev[i][j]/(echos-1));
}

float4 normalize_echo_old(float4 val, int band, int delay)
{
  val-=echo_mean[band][delay];
  if (echo_dev[band][delay]>0)
    val/=echo_dev[band][delay];
  return val;
}

void statistics_first_pass(Song * song)
{
  mean_spectrum(song->get_spectrum());
  //  mean_echo(song->get_histogram());
}

void statistics_prepare_second_pass()
{
  stop_mean_spectrum();
  //  stop_mean_echo();
}

void statistics_second_pass(Song * song)
{
  dev_spectrum(song->get_spectrum());
  //  dev_echo(song->get_histogram());
}

void statistics_stop_second_pass()
{
  stop_dev_spectrum();
  // stop_dev_echo();
}

float4 stats_get_freq(int band)
{
  assert(band<spectrum_size && band >=0);
  return means[band];
}

float4 stats_get_band_freq(int band)
{
  return (barkbounds[band]+barkbounds[band+1])/2.0;
}

float4 stats_get_freq_dev(int band)
{
  return devs[band];
}

float4 stats_get_echo_old(int band, int delay)
{
  return echo_mean[band][delay];
}

int bit_rev(int in, int check)
{
  int out = 0;
  while(check>1)
    {
      out*=2;
      if (in % 2) out++;
      in/=2;
      check/=2;
    }
  return out;
}

Statistics::Statistics(QWidget *parent, DataBase *db): 
  QDialog(parent), database(db)
{
  setupUi(this);
}

void Statistics::update()
{
  const vector<Song*> all = database->getAllSongs();
  if (all.size()==0) 
    {
      status->setText("No songs indexed yet");
      return;
    }
    
  //---------------------------------------
  //    Music body size
  //---------------------------------------
  // statistics->bodysize->setText(QString("Music body size ")+
  // readable(total_music_body));
  
  //---------------------------------------
  //    The mean spectrum characteristics
  //---------------------------------------
  // the frequency means and deviations
  QPixmap *pm = new QPixmap(spectrum_size,200);
  QPainter p;
  p.begin(pm);
  QRect r(0,0,spectrum_size,200);
  p.fillRect(r,Qt::white);
  spectrum_freq min_freq = 1000;
  spectrum_freq max_freq = -1000;
  for (int j = 0; j < spectrum_size ; j++)
    {
      float4 energy_dev = stats_get_freq_dev(j) * 3.0;
      float4 energy_fix = stats_get_freq(j);
      if (energy_fix-energy_dev<min_freq) min_freq = energy_fix-energy_dev;
      if (energy_fix+energy_dev>max_freq) max_freq = energy_fix+energy_dev;
    }
  
  QString data_text = "Band\tEnergy (dB)\tVariance (dB)\n";
  if (max_freq!=-1000)
    {
      float4 dbf = 199.0/(max_freq-min_freq);  //pixels per dB
      float4 y = 199.0;
      p.setPen(Qt::green);
      do
	{
	  p.drawLine(0,(int)y,spectrum_size-1,(int)y);
	  y-=3*dbf;
	}
      while(y>=0);
      p.setPen(Qt::black);
      for (int j = 0; j < spectrum_size ; j++)
	{
	  float4 energy_fix = stats_get_freq(j);
	  data_text += QString::number(stats_get_band_freq(j));
	  data_text += "\t";
	  data_text += QString::number(energy_fix);
	  data_text += "\t";
	  data_text += QString::number(stats_get_freq_dev(j));
	  data_text +="\n";
	  energy_fix -= min_freq;
	  
	  // calculate the position of the upper side of the dev
	  float4 up = energy_fix + stats_get_freq_dev(j)*3;
	  float4 down = energy_fix - stats_get_freq_dev(j)*3;
	  for(int k = (int)(down * dbf) ; k < (int)(up * dbf) ; k ++)
	    {
	      float4 l = (float4)k/dbf;
	      float4 v = fabs(l - energy_fix);
	      v*=255.0/(stats_get_freq_dev(j)*3);
	      if (v>255) v = 255;
	      p.setPen(QColor((int)v,(int)v,(int)v));
	      p.drawPoint(j,199-k);
	    }
	  
	  p.setPen(Qt::red);
	  p.drawPoint(j,(int)(199.0-dbf*energy_fix));
	}
    }
  p.end();
  spectrum->setPixmap(*pm);
  spectrum_data->setText(data_text);

  // execute the dialog box
  status->setText("");
}

void SongSelectorLogic::openStatistics()
{
  Statistics dialog(this,database);
  dialog.exec();
}
#endif // __loaded__statistics_cpp__
