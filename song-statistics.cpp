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

#include <assert.h>
#include <qstring.h>
#include <stdio.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qtextedit.h>
#include <qlabel.h>
#include "song.h"
#include "histogram-type.h"
#include "echo-property.h"
#include "songselector.logic.h"
#include "spectrum-type.h"
#include "signals.h"
#include "files.h"
#include "statistics.h"
#include "memory.h"

spectrum_freq devs[spectrum_size] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
spectrum_freq means[spectrum_size] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static int spectra = 0;

float8 **echo_mean=NULL;
float8 **echo_dev=NULL;

static int echos = 0;

/* in comparison to pre2.7 versions, the spectrum scale is now calculated differently
 * first we take the mean value. Based on this we also take the mean variation
 * rescaling happend by subtracting the mean and then dividing by the standard deviation
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
      float d = spectrum->band(i)-means[i];
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
      echo_mean=allocate(spectrum_size,float8*);
      echo_dev=allocate(spectrum_size,float8*);
      for(int i = 0 ; i < spectrum_size ; i ++)
	{
	  echo_mean[i]=allocate(echo_prop_sx,float8);
	  echo_dev[i]=allocate(echo_prop_sx,float8);
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
	  float d = echo.get_freq_energy_probability(i,j)-echo_mean[i][j];
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

void SongSelectorLogic::openStatistics()
{
  StatisticsDialog * statistics = new StatisticsDialog(this);
  
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
      float energy_dev = stats_get_freq_dev(j) * 3.0;
      float energy_fix = stats_get_freq(j);
      if (energy_fix-energy_dev<min_freq) min_freq = energy_fix-energy_dev;
      if (energy_fix+energy_dev>max_freq) max_freq = energy_fix+energy_dev;
    }
  
  QString data_text = "";
  if (max_freq!=-1000)
    {
      float dbf = 199.0/(max_freq-min_freq);  //pixels per dB
      float y = 199.0;
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
	  float energy_fix = stats_get_freq(j);
	  data_text += QString::number(stats_get_band_freq(j));
	  data_text += "\t";
	  data_text += QString::number(energy_fix);
	  data_text +="\n";
	  energy_fix -= min_freq;
	  
	  // calculate the position of the upper side of the dev
	  float up = energy_fix + stats_get_freq_dev(j)*3;
	  float down = energy_fix - stats_get_freq_dev(j)*3;
	  for(int k = (int)(down * dbf) ; k < (int)(up * dbf) ; k ++)
	    {
	      float l = (float)k/dbf;
	      float v = fabs(l - energy_fix);
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
  statistics->spectrum->setPixmap(*pm);
  statistics->spectrum_data->setText(data_text);

  // the echo characteristics
  /*  pm = new QPixmap(echo_prop_sx, spectrum_size);
  p.begin(pm);
  min_freq = 1000;
  max_freq = -1000;
  for (int j = 0; j < spectrum_size ; j++)
    for (int i = 0 ; i < echo_prop_sx ; i++)
      {
	float energy_fix = stats_get_echo(j,i);
	if (energy_fix<min_freq) min_freq = energy_fix;
	if (energy_fix>max_freq) max_freq = energy_fix;
      }
  
  if (max_freq!=-1000)
    {
      float4 cr = 255/(max_freq - min_freq);
      for (int j = 0; j < spectrum_size ; j++)
	for (int i = 0; i < echo_prop_sx ; i++)
	  {
	    float energy_fix = stats_get_echo(j,i);
	    energy_fix -= min_freq;
	    energy_fix *= cr;
	    int v = (int)energy_fix;
	    p.setPen(QColor(v,v,v));
	    p.drawPoint(i,spectrum_size-1-j);
	  }
    }
  p.end();
  statistics->echo->setPixmap(*pm);
  */

  // execute the dialogbox
  statistics->exec();
  delete statistics;
}
