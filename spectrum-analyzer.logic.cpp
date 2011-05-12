/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include <fftw3.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "memory.h"
#include "spectrum-analyzer.logic.h"
#include "sys/times.h"
#include "fourier.h"
#include "kbpm-play.h"
#include "version.h"
#include "scripts.h"
#include "signals.h"
#include "smallhistogram-type.h"
#include "histogram-property.h"

SpectrumDialogLogic::SpectrumDialogLogic(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  SpectrumDialog(0,name,modal,f)
{
  // clear the image
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
}

static double tovol(double a)
{
  double r = 10*log(a)/log(10);
  if (isnan(r)) return -1;
  if (r<-60) return -1;
  return r+60;
}

void fft_to_bark(double * in_r, int window_size, double * out)
{
  for(int b = 0 ; b < barksize ; b ++)
    {
      int a = (int)barkbounds[b];
      int c = (int)barkbounds[b+1];
      a *= window_size;
      c *= window_size;
      a /= WAVRATE;
      c /= WAVRATE;
      if (c==a)
	{
	  printf("a = %d; b=%d, c=%d\n",a,b,c);
	  assert(c!=a);
	}
      // bin * WAVRATE / window_size = freq; thus...
      // bin = freq * window_size / WAVRATE

      double r = 0;
      for(int i = a ; i < c; i++)
	r+= fabs(in_r[i]); // the use of only the cosine compacts the energy
      r /= c-a;
      out[b]=tovol(r);
    } 
}

static fftw_plan plan;
static double *fft_out = NULL;
static double *fft_in = NULL;

static double* init_bark_fft(int window_size)
{
  fft_out = bpmdj_allocate(window_size,double);
  fft_in = bpmdj_allocate(window_size,double);
  plan = fftw_plan_r2r_1d(window_size,fft_in,fft_out,FFTW_R2HC,FFTW_MEASURE);
  return fft_in;
}

static void free_bark_fft()
{
  fftw_free(plan);
}

static void bark_fft(int window_size, double * bark_energy)
{
  fftw_execute(plan);
  fft_to_bark(fft_out,window_size,bark_energy);
}

// the histogram can be used to see how the energy is used within a band. Where the 
// central peaks are and how quickly they fall off. 
void SpectrumDialogLogic::fetchSpectrum_normal()
{
  const int window_size = 2048;
  const int step_size = window_size/2;
  const int distri_size = 768;
  const int show_size_pattern = 96;
  const int show_size_histo = 256;
  // const int barksize = window_size/2;
  // allocate empty arrays
  double *  bark_energy = bpmdj_allocate(barksize,double);
  double * fft_in = init_bark_fft(window_size);
  // we need to go two times through the entire file 2 times. First to find the maxima
  // then to obtain the distribution
  FILE * raw = openCoreRawFile();
  long int fs = fsize(raw)/4;
  stereo_sample2 * block = bpmdj_allocate(window_size, stereo_sample2);
  histogram_type * * bark_energy_distri = bpmdj_allocate (barksize,histogram_type *);
  for(int i = 0 ; i < barksize ; i++)
    bark_energy_distri[i] = new histogram_type(0.0,96,distri_size); // 96 dB should suffice no ?
  long indicate = fs/step_size;
  indicate/=100;
  for(long int pos = 0 ; pos < fs ; pos += step_size)
    {
      if (pos%indicate==0)
	{
	  Progress->setProgress(pos,fs);
	  app->processEvents();
	}
      // read in a block
      if (pos+window_size>fs) break;
      fseek(raw,pos*sizeof(stereo_sample2),SEEK_SET);
      int read = readsamples(block,window_size,raw);
      assert(read>0);
      // convert it to the appropriate format
      for(int i = 0 ; i < window_size ; i ++)
	fft_in[i]=(double)block[i].left/32768.0;
      bark_fft(window_size,bark_energy);
      for(int j = 0 ; j < barksize ; j++)
	bark_energy_distri[j]->hit(bark_energy[j]);
      // do the same for right
      for(int i = 0 ; i < window_size ; i ++)
	fft_in[i]=(double)block[i].right/32768.0;
      bark_fft(window_size,bark_energy);
      for(int j = 0 ; j < barksize ; j++)
	bark_energy_distri[j]->hit(bark_energy[j]);
    }
  Progress->setProgress(1,1);
  fclose(raw);
  
  // write out mean spectra to file
  spectrum_type *spectrum = new spectrum_type();
  for(int i = 0 ; i < barksize ; i ++)
    spectrum->set_band(i,bark_energy_distri[i]->mean());
  playing->set_spectrum(spectrum);

  // determination of dB range
  for(int i = 0 ; i < barksize ; i ++)
    bark_energy_distri[i]->normalize(255);
  double dBmin = 0, dBmax = 0;
  for(int i = 0 ; i < barksize ; i++)
    {
      // printf("%d : %g\n",i,bark_energy_distri[i]->mean());
      double d = bark_energy_distri[i]->dev() * 3;
      if (- d < dBmin || i == 0) dBmin = - d;
      if (+ d > dBmax || i == 0) dBmax = + d;
    }
  // now draw the different distribution on the different axis
  QPixmap *pm = new QPixmap(barksize, show_size_histo);
  QPainter p;
  p.begin(pm);
  for(int x = 0 ; x < barksize ; x++)
    {
      for(int y = 0 ; y < show_size_histo ; y++)
	{
	  double dB = y * (dBmax - dBmin) / show_size_histo;
	  dB+=dBmin;
	  dB+=bark_energy_distri[x]->mean();
	  int m = bark_energy_distri[x]->valat(dB);
	  QColor col;
	  col.setRgb(m,m,m);
	  p.setPen(col);
	  p.drawPoint(x,show_size_histo-y-1);
	}
    }
  p.end(); 
  histo->setPixmap(*pm);
  
  // normalisation usable for distance measurment  
  for(int i = 0 ; i < barksize ; i ++)
    {
      bark_energy_distri[i]->normalize_autocorrelation_diff(255);
      bark_energy_distri[i]->halve();
      bark_energy_distri[i]->halve();
      bark_energy_distri[i]->strip();
    }

  // set the current data of the playing song
  echo_property h(bark_energy_distri);
  playing->set_histogram(h);

  // now draw the different distribution on the different axis
  pm = new QPixmap(barksize, show_size_pattern);
  p.begin(pm);
  for(int x = 0 ; x < barksize ; x++)
    for(int y = 0 ; y < show_size_pattern ; y++)
      {
	int m=0;
	if (y<bark_energy_distri[x]->count)
	  m = bark_energy_distri[x]->bins[y];
	if (m<0)
	  p.setPen(QColor(-m,-m,-m));
	else
	  p.setPen(QColor(m,0,0));
	p.drawPoint(x,y);
      }
  p.end(); 
  distri->setPixmap(*pm);
  // deallocate everything
  free_bark_fft();
  for(int i = 0 ; i < barksize ; i ++)
    delete bark_energy_distri[i];
  bpmdj_deallocate(bark_energy_distri);
  bpmdj_deallocate(fft_in);
  bpmdj_deallocate(block);
}
