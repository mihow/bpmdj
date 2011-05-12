/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
using namespace std;
#line 1 "statistics.c++"
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

spectrum_freq devs[spectrum_size] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
spectrum_freq means[spectrum_size] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// unsigned8 total_music_body = 0;
static int spectra = 0;

float8 **echo_mean=NULL;
float8 **echo_dev=NULL;

static int echos = 0;

/**
 * in comparison to pre2.7 versions, the spectrum scale is now calculated differently
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


static int compare_int(const void * a, const void * b)
{
  assert(a);
  assert(b);
  int A = *((int *)a);
  int B = *((int *)b);
  A%=1000;
  B%=1000;
  return A < B ? -1 : (A > B ? 1 : 0);
}

Statistics::Statistics(QWidget *parent, DataBase *db): QDialog(parent), database(db)
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
  //  statistics->bodysize->setText(QString("Music body size ")+readable(total_music_body));
  
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
      float energy_dev = stats_get_freq_dev(j) * 3.0;
      float energy_fix = stats_get_freq(j);
      if (energy_fix-energy_dev<min_freq) min_freq = energy_fix-energy_dev;
      if (energy_fix+energy_dev>max_freq) max_freq = energy_fix+energy_dev;
    }
  
  QString data_text = "Band\tEnergy (dB)\tVariance (dB)\n";
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
	  data_text += "\t";
	  data_text += QString::number(stats_get_freq_dev(j));
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
  spectrum->setPixmap(*pm);
  spectrum_data->setText(data_text);

  //---------------------------------------
  //    The echo characteristics
  //---------------------------------------
  // 1- copy all data into a new array
  status->setText("1- copying all data");
  app->processEvents();

  float * * echo_stack = bpmdj_allocate(all.size(),float *);
  int vec_size = spectrum_size*echo_prop_sx;
  int nr = 0;
  constVectorIterator<Song*> song(all); ITERATE_OVER(song)
    echo_property echo = song.val()->get_histogram();
    if (!echo.empty())
    {
      echo_stack[nr] = bpmdj_allocate(vec_size,float);
      for(int x = 0 ; x < echo_prop_sx ; x++)
	for(int y = 0 ; y < spectrum_size ; y++)
	  echo_stack[nr][x+y*echo_prop_sx]=echo.get_freq_energy_probability_scaled(y,x);
      nr++;
    }
  }
  echo_stack = bpmdj_reallocate(echo_stack,nr,float*);
  printf("   we have %d elements\n",nr);

  // 2 - normalize the mean value for every element
  status->setText("2- translating mean");
  app->processEvents();

  float * mean = bpmdj_allocate(vec_size, float);
  for(int i = 0 ; i < vec_size ; i ++)
    mean[i]=0;
  for(int i = 0 ; i < nr ; i ++)
    for(int j = 0 ; j < vec_size ; j++)
      mean[j]+=echo_stack[i][j];
  for(int j = 0 ; j < vec_size ; j++)
    mean[j]/=nr;
  for(int i = 0 ; i < nr ; i ++)
    for(int j = 0 ; j < vec_size ; j++)
      echo_stack[i][j]-=mean[j];
  
  // 3 - normalize the nr * standard deviation
  status->setText("3- normalizing standard deviation");
  app->processEvents();

  float * stddev = bpmdj_allocate(vec_size, float);
  for(int i = 0 ; i < vec_size ; i ++)
    stddev[i]=0;
  for(int i = 0 ; i < nr ; i ++)
    for(int j = 0 ; j < vec_size ; j++)
      stddev[j]+=echo_stack[i][j]*echo_stack[i][j];
  for(int j = 0 ; j < vec_size ; j++)
    stddev[j]=sqrt(stddev[j]);
  for(int i = 0 ; i < nr ; i ++)
    for(int j = 0 ; j < vec_size ; j++)
      echo_stack[i][j]/=stddev[j];
  
  // 4 - allocate the color array
  status->setText("4- allocating colors");
  app->processEvents();

  int nr_colors = 16 ;
  float * * colors = bpmdj_allocate(nr_colors , float *);
  for(int i = 0 ; i < nr_colors ; i++)
    {
      colors[i] = bpmdj_allocate(vec_size , float);
      for(int j = 0 ; j < vec_size ; j++)
	colors[i][j]=0.0;
    }
  
  // 5 - for every color select a good correlation vector
  //     and correlate with that one
  for(int c = 0 ; c < nr_colors ; c ++)
    {
      // 5a - select a position with the lowest correlations
      int x = 0;
      double v = 1.0;
      for(int i = 0 ; i < vec_size ; i ++)
	{
	  double w = 0;
	  for(int j = 0 ; j < nr_colors ; j ++)
	    w+=colors[j][i];
	  w/=nr_colors;
	  if (w<v)
	    {
	      x = i;
	      v = w;
	    }
	}

      status->setText(QString("5- correlating color ")+QString::number(c)+"/"+QString::number(nr_colors-1));
      app->processEvents();
      // printf("5- correlating with vector (%d,%d)=%g nr %d/%d\n",x%echo_prop_sx,x/echo_prop_sx,v,c,nr_colors-1);
      
      // 5b - fill in this color position with the new correlation analysis
      for(int i = 0 ; i < vec_size ; i++)
	{
	  double r = 0;
	  for(int j = 0 ; j < nr ; j++)
	    r+=echo_stack[j][i]*echo_stack[j][x];
	  colors[c][i] = r;
	}


      // 5c - we create a picture of this plane and dump it to disk 
#ifdef DUMP_PLANES
      QPixmap * pm = new QPixmap(echo_prop_sx, spectrum_size);
      p.begin(pm);
      int X = x;
      for (int y= 0; y < spectrum_size ; y++)
	for (int x = 0; x < echo_prop_sx ; x++)
	  {
	    int idx = x+y*echo_prop_sx;
	    double cv = colors[c][idx];
	    QColor C;
	    if (cv>0)
	      C.setRgb(255-cv*255,255,255-cv*255);
	    else
	      C.setRgb(255,255+cv*255,255+cv*255);
	    if (idx == X)
	      C.setRgb(0,0,255);
	    p.setPen(C);
	    p.drawPoint(x,spectrum_size-1-y);
	  }
      p.end();
      pm->save(QString("Plane"+QString::number(c)+QString(".png")),"PNG");
      delete(pm);
#endif


      // 5c - normalize the correlation result for this color slice
      /*      double m = 1.0;
      for(int i = 0 ; i < vec_size ; i ++)
	if (colors[c][i]<m) m=colors[c][i];
      for(int i = 0 ; i < vec_size ; i ++)
	colors[c][i]-=m;
      m = 0.0;
      for(int i = 0 ; i < vec_size ; i ++)
	if (colors[c][i]>m) m=colors[c][i];
      for(int i = 0 ; i < vec_size ; i ++)
	colors[c][i]/=m;
      */
      
      for(int i = 0 ; i < vec_size ; i++)
	colors[c][i] = fabs(colors[c][i]);
    }

  // 6 - determine the colors based on a PCA analysis of the colors matrix
  status->setText("6- singular value decomposition");
  app->processEvents();
  
  // 6a - copy data
  printf("Copying PCA data\n");
  float ** pca_in_out = matrix(nr_colors,vec_size/8);
  for(int y = 0 ; y < nr_colors ; y ++)
    for(int x = 0 ; x < vec_size ; x ++)
      pca_in_out[y+1][x/8+1]=colors[y][x] > 0.1 ? colors[y][x] : 0;
  
  // 6b - perform PCA
  printf("Performing PCA\n");
  char * err = NULL;
  do_pca(nr_colors,vec_size/8,pca_in_out,err);
  if (err) printf("Error %s\n",err);
  
  // 6c - translate central position
  printf("Translateing mean\n");
  double cx =0, cy=0;
  for(int y = 1 ; y <=nr_colors ; y++)
    {
      cx+=pca_in_out[y][1];
      cy+=pca_in_out[y][2];
    }
  cx/=nr_colors;
  cy/=nr_colors;
  for(int y = 1 ; y <=nr_colors ; y++)
    {
      pca_in_out[y][1]-=cx;
      pca_in_out[y][2]-=cy;
    }
  
  // 6d - determine angle
  printf("Determining angle\n");
  int * tmphue = bpmdj_allocate(nr_colors,int);
  for(int y = 1 ; y <= nr_colors ; y++)
    {
      pca_in_out[y][3]=atan2(pca_in_out[y][2],pca_in_out[y][1]);
      pca_in_out[y][3]*=180;
      pca_in_out[y][3]/=M_PI;
      pca_in_out[y][3]+=180.0;
      tmphue[y-1]=((int)pca_in_out[y][3])+1000*y;
      printf("Angle %d: %g\n",y,pca_in_out[y][3]);
    }
  // we have a set of angles for every color, we now want
  // to make the contrast between every two angles as large
  // as possible by hussling 'em around
  qsort(tmphue,nr_colors,sizeof(int),compare_int);
  // the new tmphue array has the sorted indices if we divide it by 
  // 1000. So c=tmphue[idx]/1000 specifies that c is the color which
  // is enumerated at idx. We need to reverse this
  int *hue = bpmdj_allocate(nr_colors,int);
  for(int i = 0 ; i < nr_colors ; i++)
    {
      int j = (tmphue[i]/1000)-1;
      assert(j>=0);
      assert(j<nr_colors);
      hue[j]=i;
    }
  bpmdj_deallocate(tmphue);
  
  // 6d - to determine the colors for every position we select which
  //     correlation vectort had the most influence

  status->setText("Preparing Pixmap");
  app->processEvents();
  
  /*  float * values =  bpmdj_allocate(vec_size, float);
  float m = mean[0];
  for(int i = 1 ; i < vec_size ; i ++)
    if (mean[i]<m) m = mean[i];
  for(int i = 1 ; i < vec_size ; i ++)
    values[i]=mean[i]-m;
  m = values[0];
  for(int i = 1 ; i < vec_size ; i ++)
    if (values[i]>m) m = values[i];
  for(int i = 1 ; i < vec_size ; i ++)
    values[i]/=m;
  // noramlisation of the stddev
  double stddev_max = 0;
  for(int j = 0 ; j < vec_size ; j++)
    if (stddev[j]>stddev_max) stddev_max=stddev[j];
  */
  
  pm = new QPixmap(echo_prop_sx, spectrum_size);
  p.begin(pm);
  for (int y= 0; y < spectrum_size ; y++)
    for (int x = 0; x < echo_prop_sx ; x++)
      {
	int idx = x+y*echo_prop_sx;
	// int col = 0;
	// double m = 0;
	double r, g, b;
	r = g = b= 0;
	QColor C;
	for(int c = 0 ; c < nr_colors ; c++)
	  {
	    float v = colors[c][idx];
	    v*=v;
	    int cp = hue[c]; // we need to know the position in the sorting
	    cp = bit_rev(cp,nr_colors);
	    assert(cp<nr_colors);
	    float color = 360.0 * (float)cp/ (float)nr_colors;
	    C.setHsv((int)color,255,(int)(v*255.0));
	    int R,G,B;
	    C.getRgb(&R,&G,&B);
	    r+=R;
	    g+=G;
	    b+=B;
	  }
	
	float m = std::max(r,std::max(g,b));
	r *= 255.0 / m;
	g *= 255.0 / m;
	b *= 255.0 / m;
	
	int H,S,V;
	C.setRgb((int)r,(int)g,(int)b);
	C.getHsv(&H,&S,&V);

	int my = spectrum_size-1;
	
	// the fake color
	C.setHsv(H,S,255);
	p.setPen(C);
	p.drawPoint(x,my-y);
      }
  p.end();
  echo->setPixmap(*pm);

  // - free all data used to analyze echo characteristic
  free_matrix(pca_in_out,nr_colors,vec_size/8);

  // execute the dialogbox
  status->setText("");
}

void SongSelectorLogic::openStatistics()
{
  Statistics dialog(this,database);
  dialog.exec();
}
