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
#include "memory.h"
#include "rythm-analyzer.logic.h"
#include "rythm-property.h"
#include "fourier.h"
#include "kbpm-play.h"
#include "version.h"
#include "scripts.h"
#include "signals.h"
#include <fftw3.h>
#include <qmessagebox.h>

RythmDialogLogic::RythmDialogLogic(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  RythmDialog(0,name,modal,f)
{
  // clear the image
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
  rythm->setPixmap(*pm);
}

static double tovol(double a)
{
  double r = 10*log(a)/log(10);
  if (isnan(r)) return 0;
  if (r<-30) return 0;
  return r+30;
}

void write_out_projection(long slice_size, unsigned4* phases, int maximum_slice, const char* target)
{
  FILE * f = openRawFile(playing,arg_rawpath);
  stereo_sample2 *slice = allocate(slice_size,stereo_sample2);
  double *pattern_l = allocate(slice_size,double);
  double *pattern_r = allocate(slice_size,double);
  for(int y = 0 ; y < slice_size ; y++)
    pattern_l[y] = pattern_r[y] = 0;
  for(int x = 0 ; x < maximum_slice ; x++)
    {
      readsamples(slice,slice_size,f);
      for(int y = 0 ; y < slice_size ; y++)
	{
	  int z = ( y + phases[x] ) % slice_size;
	  pattern_l[y]+=slice[z].left;
	  pattern_r[y]+=slice[z].right;
	}
    }
  // first we remove the 'noise'
  // this is done by obtaining the mean spectrum
  
  /*  static int size = 512;
  double * mean_fft_energy = allocate(size,double);
  for(int i = 0 ; i < size ; i ++)
    mean_fft_energy[i]=0;
  double * amp = allocate(size,double);
  double * ro = allocate(size,double);
  double * io = allocate(size,double);
  double * neglect = allocate(size,double);
  int ffts_done = 0;
  for(int x = 0 ; x < slice_size ; x+=size)
    {
      // copy data
      for(int y = x ; y < x + size ; y++)
	if (y<slice_size)
	  amp[y-x] = pattern_l[y] + pattern_r[y];
	else
	  amp[y-x]=0;
      // do the fft
      fft_double(size,0,amp,NULL,ro,io);
      // calculate the energy
      for(int i = 0 ; i < size ; i++)
	{
	  double e = sqrt(ro[i]*ro[i]+io[i]*io[i]);
	  mean_fft_energy[i]+=e;
	}
      ffts_done++;
    }
  for(int i = 0 ; i < size ; i++)
    mean_fft_energy[i]/=ffts_done;
  // go again through the data and increase steapness of everything below mean
  for(int x = 0 ; x < slice_size ; x+=size)
    {
      // copy data
      for(int y = x ; y < x + size ; y++)
	if (y<slice_size)
	  amp[y-x] = pattern_l[y] + pattern_r[y];
	else 
	  amp[y-x]=0;
      // do the fft
      fft_double(size,0,amp,NULL,ro,io);
      // calculate the energy
      for(int i = 0 ; i < size ; i++)
	{
	  double e = ro[i] * ro[i] + io[i] * io[i];
	  double ea = sqrt(e);
	  double an = atan2(io[i],ro[i]);
	  double m = mean_fft_energy[i];
	  ea/=m;

	  double E = ((ea/m)-1.0);
	  if (E>1) E=1; 
	  if (E<-1) E=-1;
	  E*=M_PI/2.0;
	  E = sin(E);
	  E += 1.0;
	  E /= 2.0;

	  ea = pow(ea,E);
	  	  
	  ro[i]=ea*cos(an);
	  io[i]=ea*sin(an);
	}
      // do the inverse transofmration
      fft_double(size,1,ro,io,amp,neglect);
      // copy data back
      for(int y = x ; y < x + size ; y++)
	if (y<slice_size)
	  {
	    pattern_l[y]  = amp[y-x];
	    pattern_r[y]  = amp[y-x];
	  }
    }
  */

  // now we normalize the whole
  normalize_abs_max(pattern_l,slice_size);
  normalize_abs_max(pattern_r,slice_size);
  
  deallocate(slice);
  // normalize the energy content to the required level
  FILE * out = fopen(target,"wb");
  for(int y = 0 ; y < slice_size ; y++)
    {
      signed2 t = (signed2)(pattern_l[y]*32767.0);
      fwrite(&t,2,1,out);
      t = (signed2)(pattern_r[y]*32767.0);
      fwrite(&t,2,1,out);
    }
  for(int y = 0 ; y < slice_size ; y++)
    {
      signed2 t = (signed2)(pattern_l[y]*32767.0);
      fwrite(&t,2,1,out);
      t = (signed2)(pattern_r[y]*32767.0);
      fwrite(&t,2,1,out);
    }
  for(int y = 0 ; y < slice_size ; y++)
    {
      signed2 t = (signed2)(pattern_l[y]*32767.0);
      fwrite(&t,2,1,out);
      t = (signed2)(pattern_r[y]*32767.0);
      fwrite(&t,2,1,out);
    }
  for(int y = 0 ; y < slice_size ; y++)
    {
      signed2 t = (signed2)(pattern_l[y]*32767.0);
      fwrite(&t,2,1,out);
      t = (signed2)(pattern_r[y]*32767.0);
      fwrite(&t,2,1,out);
    }
  fclose(out);
  fclose(f);
}

void fft_to_bark(double * in_r, int window_size, spectrum_type &out)
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
      out.set_bark(b,0);
      double r = 0;
      for(int i = a ; i < c; i++)
	r+=fabs(in_r[i]);
      out.set_bark(b,r/(c-a));
    } 
}

static fftw_plan plan;
static double *fft_out = NULL;
static double *fft_in = NULL;
static double *init_bark_fft2(int window_size)
{
  fft_in  = allocate(window_size,double);
  fft_out = allocate(window_size,double);
  plan = fftw_plan_r2r_1d(window_size,fft_in,fft_out,FFTW_R2HC,FFTW_MEASURE);
  return fft_in;
}

static void bark_fft2(int window_size, spectrum_type &bark_energy)
{
  fftw_execute(plan);
  fft_to_bark(fft_out,window_size,bark_energy);
}

static void free_bark_fft2()
{
  fftw_free(plan);
}

void RythmDialogLogic::calculateRythmPattern2()
{
  // check premises
  if (!normalperiod)
    { 
      QMessageBox::critical(NULL,
			    "Rhythm analyzer",
			    QString("Need tempo before rhythm can be analyzed"),
			    QMessageBox::Ok,0,0);
      return;
    }
  spectrum_type * spectrum = playing->get_spectrum();
  if (!spectrum)
    {
      QMessageBox::critical(NULL,
			    "Rhythm analyzer",
			    QString("Need spectrum before rhythm can be analyzed"),
			    QMessageBox::Ok,0,0);
      return;
    }
  // constants and some variables
  const int window_size = 2048;
  const int view_xs = 640;
  // how many frames will go into the period of the song
  int slice_samples = normalperiod * beats->value() / 4;
  int slice_frames = 192;
  // allocate arrays
  stereo_sample2 * slice_audio = allocate(slice_samples,stereo_sample2);
  spectrum_type * slice_freq = allocate(slice_frames,spectrum_type);
  spectrum_type * slice_prot = allocate(slice_frames,spectrum_type);
  spectrum_type * last_measure = allocate(slice_frames,spectrum_type);
  for(int i = 0 ; i < slice_frames ; i ++)
    for(int b = 0 ; b < 24 ; b++)
      {
	slice_prot[i].set_bark(b,0);
	last_measure[i].set_bark(b,0);
      }
  double * buffer_fft = init_bark_fft2(window_size);
  // open song
  FILE * raw = openRawFile(playing, arg_rawpath);
  long int audio_size = fsize(raw)/4;
  int max_slices = audio_size / slice_samples;
  spectrum_type * changes = allocate(max_slices,spectrum_type);
  for(int i = 0 ; i < max_slices ; i ++)
    for(int b = 0 ; b < 24 ; b ++)
      changes[i].set_bark(b,0);
  unsigned4 * phases = allocate(max_slices,unsigned4);
  // go through song slice by slice
  status_bar->setText("Calibrating");
  for(int slice = 0 ; slice < max_slices ; slice++)
    {
      Progress->setProgress(slice,max_slices);
      if (slice==1) status_bar->setText("Processing");
      app->processEvents();
      // read data
      long int pos = slice * slice_samples;
      fseek(raw,pos*sizeof(stereo_sample2),SEEK_SET);
      assert(pos+slice_samples <= audio_size);
      int read = readsamples(slice_audio,slice_samples,raw);
      assert(read>0);
      for(int a = 0 ; a < slice_frames ; a ++)
	{
	  // take the appropriate data
	  int s = a * ( slice_samples - window_size )/ slice_frames;
	  assert(s + window_size < slice_samples);
	  for(int i = 0 ; i < window_size ; i++)
	    buffer_fft[i]=slice_audio[i+s].left+slice_audio[i+s].right;
	  // convert to fft and normalize
	  bark_fft2(window_size,slice_freq[a]);
	  for(int i = 0 ; i < 24 ; i ++)
	    {
	      double v;
	      v = slice_prot[a].get_bark(i);
	      assert(!isinf(v));
	      v = tovol(slice_freq[a].get_bark(i));
	      assert(!isinf(v));
	      slice_freq[a].set_bark(i,v);
	    }
	}
      // find best rotational fit towards prototype
      double m = HUGE;
      int a = 0;
      for(int p = 0 ; p < slice_frames ; p++)
	{
	  double d = 0;
	  for(int y = 0 ; y < slice_frames ; y++)
	    {
	      int z = (y + p)%slice_frames;
	      for(int b = 0 ; b < 24 && d < m; b ++)
		{
		  double v = slice_prot[y].get_bark(b) / slice;
		  double w = slice_freq[z].get_bark(b);
		  d += (v-w) * (v-w);
		}
	    }
	  if (d<m)
	    {
	      m = d;
	      a = p;
	    }
	}
      phases[slice]=a * ( slice_samples - window_size )/ slice_frames;
      // add the rotation to the prototype
      for(int y = 0 ; y < slice_frames; y++)
	{
	  int z = (y + a)%slice_frames;
	  for(int b = 0 ; b < 24 ; b++)
	    slice_prot[y].set_bark(b,slice_prot[y].get_bark(b)+slice_freq[z].get_bark(b));
	}
      // find best rotational fit tov last measure
      if (slice>0)
	{
	  // store last measure and remember change level
	  for(int b = 0 ; b < 24 ; b++)
	    {
	      double d = 0;
	      for(int y = 0 ; y < slice_frames; y++)
		{
		  // double v = last_measure[y].get_bark(b);
		  double w = slice_freq[y].get_bark(b);
		  d += /* v - */ w;
		}
	      // 	      d = log(d);
	      if (d<0) d = 0;
	      changes[slice].set_bark(b,d);
	    }
	}
      for(int p = 0 ; p < slice_frames ; p++)
	for(int q = 0 ; q < 24 ; q ++)
	  last_measure[p].set_bark(q,slice_freq[p].get_bark(q));
    }
  Progress->setProgress(1,1);
  fclose(raw);
  
  if (!outfile->text().isEmpty())
    {
      status_bar->setText("Writing out projection");
      app->processEvents();
      write_out_projection(slice_samples,phases,max_slices,outfile->text());
    }
  
  status_bar->setText("Normalizing");
  app->processEvents();

  // normalize the spectrum
  for(int f = 0 ; f < slice_frames ; f++)
    for(int b = 0 ; b < 24 ; b ++)
      slice_prot[f].set_bark(b,slice_prot[f].get_bark(b)/max_slices);
  spectrum_type maxima;
  spectrum_type mean;
  spectrum_type scale;
  for(int f = 0 ; f < slice_frames ; f++)
    for(int b = 0 ; b < 24 ; b ++)
      {
	double v = slice_prot[f].get_bark(b);
	mean.set_bark(b,mean.get_bark(b)+v);
	if (fabs(v) > maxima.get_bark(b))
	  maxima.set_bark(b,fabs(v));
      }
  for(int f = 0 ; f < slice_frames ; f++)
    for(int b = 0 ; b < 24 ; b ++)
      {
	double v = slice_prot[f].get_bark(b);
	double m = mean.get_bark(b) / slice_frames;
	v -=m;
	v /= maxima.get_bark(b) - m;
	scale.set_bark(b,maxima.get_bark(b) - m);
	slice_prot[f].set_bark(b,v);
      }

  // now we must find a good start on the music.
  // the start is the place after which most energy is packed 
  // as well now as in the longer future. 
  // so we take 1/16the surface and add it with 1/8the and 1/4th and 1/2
  int startpos = 0;
  double startenergy = 0;
  status_bar->setText("Locating start");
  app->processEvents();
  for(int a = 0 ; a < slice_frames ; a++)
    {
      double total = 0;
      for(int l = slice_frames/16 ; l < slice_frames ; l*=2)
	{
	  for(int f = 0 ; f < l ; f++)
	    {
	      int z = (a+f)%slice_frames;
	      for(int b = 0 ; b < 24 ; b ++)
		{
		  double v = slice_prot[z].get_bark(b);
		  total+=v;
		}
	    }
	}
      if (total>startenergy)
	{
	  startpos=a;
	  startenergy=total;
	}
    }

  // fix the rythm information for this file
  rythm_property R;
  R.init();
  for(int y = 0 ; y < 24 ; y++)
    {
      R.set_scale(y,scale.get_bark(y));
      for(int x = 0 ; x < rythm_prop_sx ; x ++)
	{
	  int z = x * slice_frames / rythm_prop_sx; 
	  z += startpos;
	  z %= slice_frames;
	  double dB = slice_prot[z].get_bark(y);
	  dB*=127;
	  dB+=127;
	  if (dB<0) dB=0;
	  if (dB>255) dB=255;
	  R.set_energy(x,y,(unsigned1)dB);
	}
    }
  playing->set_rythm(R);

  // now draw the different distribution on the different axis
  QPixmap *pm = new QPixmap(view_xs, 24);
  QPixmap *pr = new QPixmap(view_xs, 1);
  QPainter p, q;
  p.begin(pm);
  q.begin(pr);
  for(int x = 0 ; x < view_xs ; x++)
    {
      int sum = 0;
      for(int y = 0 ; y < 24 ; y ++)
	{
	  int z = x * (slice_frames - 1) / ( view_xs - 1 );
	  z+=startpos;
	  z%=slice_frames;
	  double dB = slice_prot[z].get_bark(y);
	  QColor col;
	  /* This is one way of coloring */
	  dB*=192;
	  dB+=63;
	  if (dB<0) dB=0;
	  col.setHsv(y*240/24,255,(int)dB);
	  /* This is another 
	     dB*=255;
	     if (dB>255) dB=255;
	     if (dB<-255) dB=-255;
	     if (dB>=0)
	     col.setRgb((int)dB,0,0);
	     else if (dB<0)
	     col.setRgb(0,0,(int)-dB);
	  */
	  sum+=(int)dB;
	  
	  p.setPen(col);
	  p.drawPoint(x,23-y);
	}
      sum /= 24;
      q.setPen(QColor(sum,sum,sum));
      q.drawPoint(x,0);
    }
  p.end(); 
  q.end();
  rythm->setPixmap(*pm);
  projection->setPixmap(*pr);

  
  // normalize the composition bar & draw it
  {
    for(int b = 0 ; b < 24 ; b++)
      changes[max_slices-1].set_bark(b,0);
    spectrum_type maxima;
    spectrum_type mean;
    // spectrum_type scale;
    for(int f = 0 ; f < max_slices ; f++)
      for(int b = 0 ; b < 24 ; b ++)
	{
	  double v = changes[f].get_bark(b);
	  mean.set_bark(b,mean.get_bark(b)+v);
	  if (fabs(v) > maxima.get_bark(b))
	    maxima.set_bark(b,fabs(v));
	}
    for(int f = 0 ; f < max_slices ; f++)
      for(int b = 0 ; b < 24 ; b ++)
	{
	  double v = changes[f].get_bark(b);
	  double m = mean.get_bark(b) / max_slices;
	  v -=m;
	  v /= maxima.get_bark(b) - m;
	  // scale.set_bark(b,maxima.get_bark(b) - m);
	  changes[f].set_bark(b,v);
	}
  }

  // draw the composition bar
  pm = new QPixmap(max_slices, 24);
  p.begin(pm);
  p.fillRect(QRect(0,0,max_slices,24),Qt::white);
  p.setPen(Qt::black);
  for(int x = 0 ; x < max_slices ; x++)
    {
      for(int y = 0 ; y < 24 ; y ++)
	{
	  double dB = changes[x].get_bark(y);
	  QColor col;
	  dB*=128;
	  dB+=127;
	  if (dB<0) dB=0;
	  col.setHsv(y*240/24,255,(int)dB);
	  p.setPen(col);
	  p.drawPoint(x,23-y);
	}
    }
  p.end(); 
  composition->setPixmap(*pm);

  // calculate the frequency content of the composition
  const int ps = 33; // we take 33 measures because the first will always be zero
  int ws = higher_power_of_two(max_slices);
  if (ps>ws) 
    ws = higher_power_of_two(ps);
  double * ain = allocate(ws,double);
  double * aou = allocate(ws,double);
  double * aio = allocate(ws,double);
  double * periods = allocate(ps,double);
  // int *counts = allocate(ps,int);
  pm = new QPixmap(ps, 24);
  p.begin(pm);
  composition_property cp;
  cp.init();
  for(int y = 0 ; y < 24 ; y ++)
    {
      for(int x = 0 ; x < max_slices ; x++)
	ain[x]=changes[x].get_bark(y);
      for(int x = max_slices - 1; x >=1 ; x--)
	ain[x]-=ain[x-1];
      ain[0]=0;
      for(int x = max_slices - 1; x < ws ; x++)
	ain[x]=0;
      unbiased_autocorrelation(ain,ws);
      for(int pml = 0 ; pml < ps ; pml++)
	periods[pml]=ain[pml];
      periods[0]=0;
      double maxed = normalize_abs_max(periods,ps);
      printf("%g\n",maxed);
      // assign the autocorrelation to the composition property
      cp.set_scale(y,maxed);
      for(int x = 1 ; x < ps ; x ++)
	{
	  double val = periods[x];
	  val*=127;
	  val+=127;
	  if (val<0) val = 0;
	  if (val>255) val = 255;
	  cp.set_energy(x-1,y,(unsigned1)val);
	}

      for(int x = 0 ; x < ps ; x++)
	{
	  double dB = periods[x];
	  QColor col;
	  if (dB<0) dB=-dB;
	  dB*=255;
	  col.setHsv(y*240/24,255,(int)dB);
	  p.setPen(col);
	  p.drawPoint(x,23-y);
	}
    }
  // the data which we will write out in the index file are the non absed periods
  deallocate(aio);
  deallocate(aou);
  deallocate(ain);
  p.end(); 
  composition_freq->setPixmap(*pm);
  playing->set_composition(cp);
  
  status_bar->setText("Done");
  free_bark_fft2();
}



