/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__bpm_analyzer_cpp__
#define __loaded__bpm_analyzer_cpp__
using namespace std;
#line 1 "bpm-analyzer.c++"
#include <qapplication.h>
#include <qradiobutton.h>
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
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <syscall.h>
#include <math.h>
#include <fftw3.h>
#include "bpm-analyzer.h"
#include "sys/times.h"
#include "version.h"
#include "player-core.h"
#include "scripts.h"
#include "bpm.h"
#include "memory.h"
#include "clock-drivers.h"

#define IMAGE_XS 320
#define IMAGE_YS 240
const signed4 shifter = 12;
const signed4 spectrum_shifter = 2;

BpmAnalyzerDialog::BpmAnalyzerDialog(QWidget*parent) : QWidget(parent)
{
  setupUi(this);
  player=NULL;
  tapcount=0;
  audio=NULL;
  audiorate = 11025;
  startbpm = 120;
  stopbpm = 160;
  startshift = 0;
  stopshift = 0;
  bufsiz = 32 * 1024;
  freq = NULL;
  reading_progress = 0;
  processing_progress = 0;
  // clear the image
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
  BpmPix->setPixmap(*pm);
}

unsigned4 BpmAnalyzerDialog::phasefit(unsigned4 i)
{
  unsigned4 c;
  unsigned4 mismatch=0;
  for(c=i;c<(unsigned4)audiosize;c++)
    mismatch+=abs((signed4)audio[c]-(signed4)audio[c-i]);
  return mismatch;
}

unsigned4 BpmAnalyzerDialog::phasefit(unsigned4 i, unsigned4 clip)
{
  unsigned4 c;
  unsigned4 mismatch=0;
  if (!clip) return phasefit(i);
  assert(audiosize>=0 && i<(unsigned4)audiosize);
  for(c=i;c<(unsigned4)audiosize && mismatch <= clip ;c++)
    mismatch+=abs((signed4)audio[c]-(signed4)audio[c-i]);
  return mismatch;
}

void BpmAnalyzerDialog::setBpmBounds(signed4 start, signed4 stop)
{
  char tmp[500];
  char tmp2[500];
  startbpm=start;
  stopbpm=stop;
  sprintf(tmp,"%d",startbpm);
  FromBpmEdit->setText(tmp);
  sprintf(tmp2,"%d",stopbpm);
  ToBpmEdit->setText(tmp2);
}

void BpmAnalyzerDialog::updateReadingProgress(signed4 val)
{
  if (reading_progress!=val)
    {
      reading_progress=val;
      updateInfo();
    }
}


void BpmAnalyzerDialog::updateProcessingProgress(signed4 val)
{
  if (processing_progress!=val)
    {
      processing_progress = val;
      updateInfo();
    }
}

void BpmAnalyzerDialog::updateInfo()
{
  ReadingBar->setValue(reading_progress);
  CountingBar->setValue(processing_progress);
  app->processEvents();
}

void BpmAnalyzerDialog::readAudio()
{
  FILE * raw = openCoreRawFile();
  audiosize=fsize(raw);
  signed4 startpercent=0;
  startpercent-=startpercent%4;
  signed4 stoppercent=audiosize;
  stoppercent-=stoppercent%4;
  audiosize=stoppercent-startpercent;
  audiosize/=(4*(WAVRATE/audiorate));
  audio=bpmdj_allocate(audiosize+1,unsigned1);
  status("Reading");
  unsigned4 pos=0;
  signed4 count, redux, i;
  signed short buffer[bufsiz];
  fseek(raw,startpercent,SEEK_SET);
  assert(audiosize>=0);
  while(pos<(unsigned4)audiosize && !stop_signal)
    { 
      // als hem 1000 zegt hebben we dus eigenlijk 2000 samples binnen gekregen
      unsigned4 *trgt = (unsigned4*)(void*)buffer;
      count = readsamples(trgt,bufsiz/2,raw);
      updateReadingProgress(pos*100/audiosize);
      for (i = 0 ; i < count * 2 ; i += 2 * (WAVRATE/audiorate) )
	{
	  signed4 left, right,mean;
	  left=abs(buffer[i]);
	  right=abs(buffer[i+1]);
	  mean=(left+right)/2;
	  redux=abs(mean)/128;
	  if (pos+i/(2*(WAVRATE/audiorate))>=(unsigned4)audiosize) break;
	  assert(pos+i/(2*(WAVRATE/audiorate))<(unsigned4)audiosize);
	  audio[pos+i/(2*(WAVRATE/audiorate))]=(unsigned1)redux;
	}
      pos+=count/(WAVRATE/audiorate);
    }
  fclose(raw);
  if (stop_signal)
    {
      bpmdj_deallocate(audio);
      audio=NULL;
      audiosize=0;
      status("Canceled while reading");
      return;
    }
  reading_progress = 100;
  updateInfo();
}

void BpmAnalyzerDialog::status(QString text)
{
  StatusLabel->setText(text);
  updateInfo();
}

void BpmAnalyzerDialog::readAudioBlock(signed4 blocksize)
{
  FILE * raw = openCoreRawFile();
  audiosize=fsize(raw);  // uitgedrukt in bytes...
  signed4 startpercent=0;
  startpercent/=4;       // uitgedrukt in sample dus...
  signed4 stoppercent=audiosize;
  stoppercent/=4;        // uitgedrukt in samples
  audiosize = stoppercent-startpercent;  // uitgedrukt in samples
  audiosize /= blocksize;        // uitgedrukt in blokken
  audio = bpmdj_allocate(audiosize, unsigned1);
  updateProcessingProgress(0);
  status("Reading audio (block size "+QString::number(blocksize)+")");
  unsigned4 pos = 0;
  signed4 count;
  stereo_sample2 buffer[blocksize];
  fseek(raw,startpercent*4,SEEK_SET);
  while((signed8)pos<(signed8)audiosize && !stop_signal)
    {
      count = 0;
      do 
	{
	  count = readsamples(buffer+count,blocksize-count,raw);
	}
      while (count < blocksize );
      updateReadingProgress(pos*100/audiosize);
      unsigned4 total = 0;
      for (signed4 i = 0 ; i < blocksize ; i ++ )
	{
	  signed4 left, right;
	  left=abs(buffer[i].left);
	  right=abs(buffer[i].right);
	  total+=(left+right)/256;
	}
      audio[pos]=total/blocksize;
      pos++;
    }
  fclose(raw);
  
  if (stop_signal)
    {
      bpmdj_deallocate(audio);
      audio=NULL;
      audiosize=0;
      status("Canceled while reading");
      return;
    }  
  reading_progress = 100;
  updateInfo();
}

void BpmAnalyzerDialog::rangeCheck()
{
  unsigned4 val;
  bool changed = false;
  val = FromBpmEdit->text().toInt();
  if (startbpm != val)
    startbpm=val;
  val = ToBpmEdit->text().toInt();
  if (stopbpm != val)
    stopbpm=val;
  if (changed && audio)
    {
	bpmdj_deallocate(audio);
	audio=NULL;
	audiosize=0;
    }
}

fft_type index2autocortempo(signed4 i)
{
  assert(i);
  fft_type measure_period_in_ticks = i<<spectrum_shifter;
  fft_type measure_period_in_secs  = measure_period_in_ticks/(fft_type)WAVRATE;
  fft_type measure_frequency_in_hz = 1/measure_period_in_secs;
  fft_type measure_frequency_in_bpm = measure_frequency_in_hz*60.0;
  fft_type beat_frequency_in_bpm = measure_frequency_in_bpm*4.0;
  return beat_frequency_in_bpm;
}

void BpmAnalyzerDialog::autocorrelate_draw(QPainter &p, signed4 xs, signed4 ys,
signed4 shifter)
{
  if (!freq) return;
  p.setPen(Qt::gray);
  // first find the upper and lower bounds of the energy spectrum
  float8 min_energy=1.0;
  float8 max_energy=0.0;
  for(signed4 i = 0 ; i < windowsize/2 ; i ++)
    {
      if (freq[i]>max_energy)
	max_energy=freq[i];
      if (freq[i]<min_energy)
	min_energy=freq[i];
    }
  max_energy-=min_energy;

  signed4 lastx = 0, lasty = 0, lastcount = 0;
  for(signed4 i = 1 ; i <windowsize/2; i ++)
    {
      fft_type bpm = index2autocortempo(i);      
      // calculate position
      signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
      signed4 y = (signed4)((float8)ys-(float8)ys*freq[i]);
      if (x<0 || x>=xs) continue;
      if (y>=ys) continue;
      if (y<0) y=0;
      
      if (x!=lastx && lastcount)
	{
	  p.drawLine(lastx,lasty/lastcount,lastx,ys);
	  lastx=x;
	  lasty=0;
	  lastcount=0;
	}
      lasty+=y;
      lastcount++;
    }
  
  for (signed4 i = 0 ; i < peaks ; i ++)
    {
      QColor c(255-255*i/peaks,0,0);
      p.setPen(c);
      float8 bpm = peak_bpm[i];
      float8 energy = peak_energy[i];
      signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
      signed4 y = (signed4)((float8)ys-(float8)ys*energy);
      QString text = QString::number(i)+") "+QString::number(bpm);
      p.drawText(x,y,text);
    }
}

void BpmAnalyzerDialog::fft_draw(QPainter &p, signed4 xs, signed4 ys, 
signed4 shifter, float8 bpm_divisor)
{
  if (!freq) return;
  p.setPen(Qt::gray);
  float8 min_energy=1.0;
  float8 max_energy=0.0;
  for(signed4 i = 0 ; i < windowsize/2 ; i ++)
    {
      if (freq[i]>max_energy)
	max_energy=freq[i];
      if (freq[i]<min_energy)
	min_energy=freq[i];
    }
  max_energy-=min_energy;
  signed4 lastx = 0, lasty = 0, lastcount = 0;
  for(signed4 i = 0 ; i <windowsize/2; i ++)
    {
      // calculate BPM
      float8 bpm = Index_to_frequency(windowsize,i);  // relative wrt WAVRATE
      bpm*=(float8)WAVRATE;                // in Hz tov non collapsed WAVRATE
      for(signed4 j = 0 ; j < shifter; j ++) bpm/=2.0;  // collapsed WAVRATE
      bpm*=60.0/bpm_divisor;                                      // in BPM.
      // calculate position
      signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
      signed4 y = (signed4)((float8)ys-(float8)ys*freq[i]);
      if (x<0 || x>=xs) continue;
      if (y>=ys) continue;
      if (y<0) y=0;
      
      if (x!=lastx)
	{
	  p.drawLine(lastx,lasty/lastcount,lastx,ys);
	  lastx=x;
	  lasty=0;
	  lastcount=0;
	}
      lasty+=y;
      lastcount++;
    }
  for (signed4 i = 0 ; i < peaks ; i ++)
    {
      QColor c(255-255*i/peaks,0,0);
      p.setPen(c);
      float8 bpm = peak_bpm[i];
      float8 energy = peak_energy[i];
      signed4 x = (signed4)((float8)xs*(bpm-startbpm)/(stopbpm-startbpm));
      signed4 y = (signed4)((float8)ys-(float8)ys*energy);
      QString text = QString::number(i)+") "+QString::number(bpm);
      p.drawText(x,y,text);
    }
}

void BpmAnalyzerDialog::fft()
{
  signed8 audiosize;
  FILE * raw = openCoreRawFile();
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // deel door 2^shifter to bring us within een range van 322.9 BPM
  audiosize>>=shifter;
  signed4 blocksize = 1 << shifter;
  stereo_sample2 *block = bpmdj_allocate(blocksize,stereo_sample2);
  float8 *audio = (float8*)fftw_malloc(audiosize*sizeof(float8));
  windowsize = lower_power_of_two(audiosize);
  freq = (fft_type*)fftw_malloc(windowsize*sizeof(fft_type));
  fftw_plan plan = fftw_plan_r2r_1d(windowsize,audio,freq,FFTW_R2HC,
				    FFTW_MEASURE);
  for(signed4 i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].left);
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("First FFT called\n");
  fftw_execute(plan);
  // rescale the entire thing
  float8 max = 0;
  signed4 halfwindow = windowsize/2;
  for(signed4 i = 0 ; i <halfwindow ; i ++)
    {
      freq[i]=10.0*log(fabs(freq[i]));
      if (freq[i]>max) max=freq[i];
    }
  for (signed4 i = 0 ; i < halfwindow ; i ++)
    freq[i]/=max;
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = bpmdj_allocate(peaks, fft_type);
  peak_energy = bpmdj_allocate(peaks, fft_type);
  fft_type *copy = bpmdj_allocate(halfwindow, fft_type);
  for(signed4 i = 0 ; i < halfwindow ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // bpm left and right...
  
  for(signed4 j = 0 ; j < peaks ; j ++)
    {
      float8 energy = 0;
      float8 at = 0;
      for(signed4 i = 0 ; i <halfwindow ; i ++)
	{
	  // obtain bpm
	  // uitgedruk relatief tov WAVRATE
	  float8 bpm = Index_to_frequency(windowsize,i);
	  // uitgedrukt in Hz tov non collapsed WAVRATE
	  bpm*=(float8)WAVRATE;                           
	  // uitgedrukt in collapsed WAVRATE
	  for(signed4 j = 0 ; j < shifter; j ++) bpm/=2.0;  
	  // uitgedrukt in BPM.
	  bpm*=60.0;                         
	  // skip or break ?
	  if (bpm<startbpm) continue;
	  if (bpm>stopbpm) break;
	  // is larger than any of the known peaks ?
	  if (copy[i]>energy)
	    {
	      energy = copy[i];
	      at = bpm;
	    }
	}
      // store peak
      peak_bpm[j]=at;
      peak_energy[j]=energy;
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      // clear neighbors
      for(signed4 i = 0 ; i <halfwindow ; i ++)
	{
	  // obtain BPM
	  // uitgedruk relatief tov WAVRATE
	  float8 bpm = Index_to_frequency(windowsize,i);         
	  // uitgedrukt in Hz tov non collapsed WAVRATE
	  bpm*=(float8)WAVRATE;            
	  // uitgedrukt in collapsed WAVRATE
	  for(signed4 j = 0 ; j < shifter; j ++) bpm/=2.0;
	  // uitgedrukt in BPM.
	  bpm*=60.0;                        
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  
  fftw_destroy_plan(plan);
  fftw_free(audio);
}

void BpmAnalyzerDialog::enveloppe_spectrum()
{
  signed8 audiosize;
  FILE * raw = openCoreRawFile();
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  audiosize>>=spectrum_shifter;
  windowsize=lower_power_of_two(audiosize);
  signed4 blocksize = 1 << spectrum_shifter;
  stereo_sample2 *block = bpmdj_allocate(blocksize,stereo_sample2);
  fft_type *audio = (fft_type*)fftw_malloc(audiosize*sizeof(fft_type));
  freq = (fft_type*)fftw_malloc(windowsize*sizeof(fft_type));
  fftw_plan plan = fftw_plan_r2r_1d(windowsize,audio,freq,FFTW_R2HC,
				    FFTW_ESTIMATE);
  for(signed4 i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].left);
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("Audio has been read...\n");
  fftw_execute(plan);
  printf("FFT has been done\n");
  // rescale the entire thing
  fft_type max = 0;
  fft_type min = -1.0;
  for(signed4 i = 0 ; i <windowsize/2 ; i ++)
    {
      fft_type bpm = Index_to_frequency(windowsize,i); // tov WAVRATE
      bpm*=(fft_type)WAVRATE;      // in Hz tov non collapsed WAVRATE
      // uitgedrukt in collapsed WAVRATE
      for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0; 
      bpm*=60.0;                                      // uitgedrukt in BPM.
      if (bpm<startbpm) continue;
      if (bpm>stopbpm) break;
      freq[i]=10.0*log(fabs(freq[i]));
      // freq[i]=fabs(freq[i]);
      if (freq[i]>max) max=freq[i];
      if (freq[i]<min || min < 0.0) min=freq[i];
    }
  printf("maximum =%g, minimum = %g\n",max,min);
  for (signed4 i = 0 ; i < windowsize / 2 ; i ++)
    freq[i]=(freq[i]-min)/(max-min);
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = bpmdj_allocate(peaks, fft_type);
  peak_energy = bpmdj_allocate(peaks, fft_type);
  fft_type *copy = bpmdj_allocate(windowsize / 2, fft_type);
  for(signed4 i = 0 ; i < windowsize/2 ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // BPM left and right...
  
  for(signed4 j = 0 ; j < peaks ; j ++)
    {
      fft_type  energy = 0, at = 0;
      for(signed4 i = 0 ; i <windowsize/2 ; i ++)
	{
	  // tov WAVRATE
	  fft_type bpm = Index_to_frequency(windowsize,i); 
	  // in Hz tov non collapsed WAVRATE
	  bpm*=(fft_type)WAVRATE; 
	  // uitgedrukt in collapsed WAVRATE
	  for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;
	  bpm*=60.0;                                      // uitgedrukt in BPM.
	  // skip or break ?
	  if (bpm<startbpm) continue;
	  if (bpm>stopbpm) break;
	  // is larger than any of the known peaks ?
	  if (copy[i]>energy)
	    {
	      energy = copy[i];
	      at = bpm;
	    }
	}
      
      // store peak
      peak_bpm[j]=at;
      peak_energy[j]=energy;
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      if (j == 0) 
	set_measured_period("Envelope",(signed4)(4.0*11025.0*60.0*4.0/at));
      
      // clear neighbors
      for(signed4 i = 0 ; i <windowsize/2 ; i ++)
	{
	  // obtain BPM
	  // relatief tov WAVRATE
	  fft_type bpm = Index_to_frequency(windowsize,i);
	  bpm*=(float8)WAVRATE;             // in Hz tov non collapsed WAVRATE
	  // in collapsed WAVRATE
	  for(signed4 j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;    
	  bpm*=60.0;                                      // in BPM.
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  fftw_destroy_plan(plan);
  fftw_free(audio);
}

void BpmAnalyzerDialog::autocorrelate_spectrum()
{
  signed8 audiosize;
  FILE * raw = openCoreRawFile();
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // OK, nu moeten we dat delen door 2^spectrum_shifter
  audiosize>>=spectrum_shifter;
  windowsize = lower_power_of_two(audiosize);
  signed4 blocksize = 1 << spectrum_shifter;
  stereo_sample2 *block = bpmdj_allocate(blocksize,stereo_sample2);
  fft_type *audio = (fft_type*)fftw_malloc(audiosize*sizeof(fft_type));
  fftw_complex* freq_tmp = (fftw_complex*)fftw_malloc(windowsize*
						      sizeof(fftw_complex));
  fftw_plan forward_plan = fftw_plan_dft_r2c_1d(windowsize,audio,freq_tmp,
						FFTW_ESTIMATE);
  freq = (fft_type*)fftw_malloc(windowsize*sizeof(fft_type));
  fftw_plan backward_plan = fftw_plan_dft_c2r_1d(windowsize,freq_tmp,freq,
						 FFTW_ESTIMATE);
  for(signed4 i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (signed4 j = 0 ; j < blocksize ; j ++)
	sum+=block[j].left;
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("Audio has been read...\n");
  
  // 1. do a FFT of the entire sample
  fftw_execute(forward_plan);
  fftw_destroy_plan(forward_plan);
  fftw_free(audio);
  printf("Forward FFT has been done\n");

  // 2. modify freq[i]=norm(freq[i])^2;
  for(signed4 i = 0 ; i < windowsize; i ++)
    {
      freq_tmp[i][0] = freq_tmp[i][0] * freq_tmp[i][0]
	+ freq_tmp[i][1] * freq_tmp[i][1];
      freq_tmp[i][1]=0;
    }
  printf("Copy has been made\n");
  
  // 3. do an inverse Fourier transform of freq[i]
  fftw_execute(backward_plan);
  fftw_destroy_plan(backward_plan);
  fftw_free(freq_tmp);
  printf("Backward FFT has been done\n");
  
  // 4. rescale & find peaks 
  // rescale the entire thing
  fft_type max = 0;
  fft_type min = -1.0;
  for(signed4 i = 1 ; i <windowsize/2 ; i ++)
    {
      fft_type bpm = index2autocortempo(i);
      if (bpm<startbpm) break;
      if (bpm>stopbpm) continue;
      freq[i]=log(fabs(freq[i]));
      // freq[i]=fabs(freq[i]);
      if (freq[i]>max) max=freq[i];
      if (freq[i]<min || min < 0.0) min=freq[i];
    }
  
  printf("maximum =%g, minimum = %g\n",max,min);
  for (signed4 i = 0 ; i < windowsize / 2 ; i ++)
    freq[i]=(freq[i]-min)/(max-min);
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = bpmdj_allocate(peaks, fft_type);
  peak_energy = bpmdj_allocate(peaks, fft_type);
  fft_type *copy = bpmdj_allocate(windowsize / 2, fft_type);
  for(signed4 i = 0 ; i < windowsize/2 ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // BPM left and right...
  
  for(signed4 j = 0 ; j < peaks ; j ++)
    {
      fft_type  energy = 0, at = 0;
      for(signed4 i = 1 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = index2autocortempo(i);
	  
	  // skip or break ?
	  if (bpm<startbpm) break;
	  if (bpm>stopbpm) continue;
	  // is larger than any of the known peaks ?
	  if (copy[i]>energy)
	    {
	      energy = copy[i];
	      at = bpm;
	    }
	}
      // store peak
      peak_bpm[j]=at;
      peak_energy[j]=energy;
      if (j == 0)
	set_measured_period("Autocorrelation",
			    (signed4)(4.0*11025.0*60.0*4.0/at));
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      
      // clear neighbors
      for(signed4 i = 1 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = index2autocortempo(i);
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm<at-range) break;
	}
    }
}

void BpmAnalyzerDialog::wec()
{
  FILE * file = openCoreRawFile();
  assert(file);
  int fd = fileno(file);
  unsigned4 map_length = fsize(file);
  stereo_sample2 * audio = (stereo_sample2*)mmap(NULL,map_length,PROT_READ,
						 MAP_SHARED,fd,0);
  assert(audio!=MAP_FAILED);
  assert(audio);
  BpmCounter bc(stderr,audio,map_length/4,WAVRATE,startbpm,stopbpm);
  tempo_type result(bc.measure());
  munmap(audio,map_length);
  set_measured_period("Wec",tempo_to_period(result).period*4);
}

void BpmAnalyzerDialog::set_labels()
{
  char d[500];
  sprintf(d,"%2g",(float8)startbpm+3.0*(float8)(stopbpm-startbpm)/4.0);
  X3->setText(d);
  sprintf(d,"%2g",(float8)startbpm+(float8)(stopbpm-startbpm)/2.0);
  X2->setText(d);
  sprintf(d,"%2g",(float8)startbpm+(float8)(stopbpm-startbpm)/4.0);
  X1->setText(d);
}

void BpmAnalyzerDialog::analyze()
{
  ::metronome->release_master();
  rangeCheck();
  set_labels();
  if (stop_signal) return;
  if (enveloppeSpectrum->isChecked())
    {
      enveloppe_spectrum();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      fft_draw(p, IMAGE_XS, IMAGE_YS, spectrum_shifter,1.0);
      p.end();
      BpmPix->setPixmap(*pm);
      return;
    }
  if (fullAutoCorrelation->isChecked())
    {
      autocorrelate_spectrum();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      autocorrelate_draw(p, IMAGE_XS, IMAGE_YS, spectrum_shifter);
      p.end();
      BpmPix->setPixmap(*pm);
      return;
    }
  if (weightedEnvCor->isChecked()) 
    {
      wec();
      return;
    }
  if (stop_signal) return;
  if (ultraLongFFT->isChecked()) 
    {
      fft();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      fft_draw(p, IMAGE_XS, IMAGE_YS, shifter,1.0);
      p.end();
      BpmPix->setPixmap(*pm);
    }
  if (stop_signal) return;
  if (resamplingScan->isChecked())
    {
      rayshoot_scan();
      return;
    }
  else
    {
      if (!audio) readAudio();
      if (stop_signal) return;
      if (ultraLongFFT->isChecked()) 
	peak_scan();
    }
}

void BpmAnalyzerDialog::rayshoot_scan()
{
  const unsigned4 blockshifter_max = 8;  
  unsigned4 blockshifter = blockshifter_max;
  signed* mismatch_array[blockshifter_max+1];
  unsigned4 mean[blockshifter_max+1];
  unsigned4 minimum_at = 0;
  unsigned4 minima[blockshifter_max+1];
  for (unsigned4 i = 0 ; i <= blockshifter_max; i++) mean[i]=0;
  unsigned4 maxima[blockshifter_max+1];
  // depending on the audiorate we should stop at a certain block shift 
  // minimum...
  unsigned4 blockshifter_min; 
  if (audiorate == 44100) blockshifter_min = 0;
  else if (audiorate == 22050) blockshifter_min = 1;
  else if (audiorate == 11025) blockshifter_min = 2;
  else assert(0);
  stopshift = 44100 * 60 * 4 / startbpm ;
  startshift = 44100 * 60 * 4 / stopbpm ;
  for( unsigned4 i = 0; i <= blockshifter_max ; i ++)
    {
      mismatch_array[i] = bpmdj_allocate(stopshift-startshift, signed);
      for(unsigned4 j = 0 ; j < stopshift-startshift ; j ++)
	mismatch_array[i][j] = -1;
    }
  while(blockshifter>=blockshifter_min && ! stop_signal)
    {
      unsigned4 blocksize = 1 << blockshifter;
      // first read audio
      readAudioBlock(blocksize);
      // calculate all mismatches
      signed * mismatch = mismatch_array[blockshifter] - startshift;
      signed * prev_mismatch = NULL;
      unsigned4 prev_maximum = 0;
      if (blockshifter < blockshifter_max)
	{
	  prev_mismatch = mismatch_array[blockshifter+1] - startshift;
	  // the mismatch array goes always from 0 to stophift-startshift-1
	  // the prev_mismatch  hence goes from  startshift to stopshift - 1
	  prev_maximum = mean[blockshifter+1]; 
	}
      status("Scanning "+QString::number(blockshifter_max-blockshifter)
	     +"/" + QString::number(blockshifter_max-blockshifter_min));
      if (!prev_mismatch)
	for (unsigned4 i = startshift ; i < stopshift && ! stop_signal; i ++ )
	  {
	    unsigned4 phase = i >> blockshifter;
	    unsigned4 store = phase << blockshifter;
	    updateProcessingProgress((i - startshift) *100 / 
				     (stopshift - startshift));
	    if (store!=i) continue;
	    unsigned4 m = phasefit(phase);
	    mismatch[store]=m;
	  }
      else
	{
	  // first a dry run to see how many times we would execute the 
	  // phasefit;
	  signed4 phasefit_called = 0;
	  signed4 phasefit_total = 0;
	  for (unsigned4 i = startshift ; i < stopshift && ! stop_signal; i++ )
	    {
	      unsigned4 phase = i >> blockshifter;
	      unsigned4 store = phase << blockshifter;
	      if (store!=i) continue;
	      unsigned4 prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned4 next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (prev_store < startshift ) continue;
	      if (next_store >= stopshift ) break;
	      // sign is important !
	      unsigned4 prev_val = prev_mismatch[prev_store]; 
	      // sign is important !
	      unsigned4 next_val = prev_mismatch[next_store]; 
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		phasefit_total++;
	    }
	  if (!phasefit_total) phasefit_total = 1;
	  // now wet run..
	  for (unsigned4 i = startshift ; i < stopshift && ! stop_signal; i++ )
	    {
	      unsigned4 phase = i >> blockshifter;
	      unsigned4 store = phase << blockshifter;
	      updateProcessingProgress(phasefit_called*100/phasefit_total);
	      if (store!=i) continue;
	      // we hebben een positie die een macht van 2 is...
	      // dus de vorige was op - en + blocksize *2
	      unsigned4 prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned4 next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (prev_store < startshift ) continue;
	      if (next_store >= stopshift ) break;
	      // sign is important !
	      unsigned4 prev_val = prev_mismatch[prev_store]; 
	      // sign is important !
	      unsigned4 next_val = prev_mismatch[next_store]; 
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		{
		  unsigned4 m = phasefit(phase);  
		  // we moeten de phase kleiner maken omdat de array
		  //  geresampled is op blocksize
		  mismatch[store]=m;
		  phasefit_called++;
		}
	    }
	}
      updateProcessingProgress(100);
      // find minimum, translate, maximum, mean
      signed minimum = mismatch[startshift];
      minimum_at = startshift;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (minimum == -1 || (mismatch[i] >= 0 && mismatch[i] < minimum))
	  minimum = mismatch[ minimum_at = i ];
      minima[blockshifter] = minimum_at;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0)
	  mismatch[i]-=minimum;
      signed maximum = -1;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0 && (mismatch [i] > maximum || maximum < 0)) 
	  maximum = mismatch[i];
      signed8 total = 0;
      signed4 count = 0;
      for (unsigned4 i = startshift ; i < stopshift ; i ++)
	if (mismatch[i]>=0)
	  {
	    count++;
	    total+=mismatch[i];
	  }
      maxima[blockshifter]=maximum;
      if (!count) count = 1;
      mean[blockshifter]=total/count;
      
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      p.setPen(Qt::gray);
      p.drawLine(IMAGE_XS/2,0,IMAGE_XS/2,IMAGE_YS);
      p.drawLine(IMAGE_XS/4,0,IMAGE_XS/4,IMAGE_YS);
      p.drawLine(IMAGE_XS*3/4,0,IMAGE_XS*3/4,IMAGE_YS);
      
      for (unsigned4 i = blockshifter_max ; i >= blockshifter ; i --)
	{
	  unsigned4 slice_maximum = maxima[i];
	  if (!slice_maximum) slice_maximum=1;
	  unsigned4 pos = r.height() - ((signed8)mean[i] * (signed8) r.height()
					/ (signed8)slice_maximum);
	  QColor c(0,0,0);
	  signed4 kleur = (i - blockshifter_min) * 128 /
	    (blockshifter_max - blockshifter_min);
	  c.setHsv(kleur, 255, 255);
	  p.setPen(c);
	  p.drawLine(0,pos,r.width(),pos);
	  float8 bpm = (float8)(4.0*60.0*44100.0)/(float8)minima[i];
	  pos = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  p.drawLine(pos,0,pos,r.height());
	  	  
	  for( unsigned4 j = startshift ; j < stopshift ; j ++ )
	    {
	      signed8 y = mismatch_array[i][j-startshift];
	      if (y > 0)
		{
		  y *= (signed8) r.height();
		  y /= (signed8) slice_maximum;
		  y = r.height() - y;
		  bpm = (float8)(4.0*60.0*44100.0)/(float8)j;
		  unsigned4 x = (signed4)((float8)r.width()*(bpm-startbpm)/
					  (stopbpm-startbpm));
		  p.drawPoint(x,y);
		  p.drawPoint(x+1,y);
		  p.drawPoint(x,y+1);
		  p.drawPoint(x+1,y+1);
		}
	    }
	}
      p.end();
      BpmPix->setPixmap(*pm);
      blockshifter --;
    }

  if (stop_signal)
    status("Canceled");
  else
    {
      status("Ready");
      set_measured_period("Rayshooting",minimum_at);
    }
}

void BpmAnalyzerDialog::set_measured_period(QString technique, signed4 p, 
bool update_on_disk)
{
  set_normalperiod(p,update_on_disk);
}

void BpmAnalyzerDialog::peak_scan()
{
  status("Finding least broken ray based on FFT");
  stopshift=audiorate*60*4/startbpm;
  startshift=audiorate*60*4/stopbpm;
  unsigned4 match[stopshift-startshift];
  for (unsigned4 i = 0 ; i < stopshift - startshift ; i ++)
    match[i]=0;
  peak_fit = bpmdj_allocate(peaks, signed4);
  for (signed4 i = 0 ; i < peaks ; i ++)
    peak_fit[i]=0;
  
  unsigned4 maximum = 0;
  unsigned4 global_minimum = 0;
  signed4 global_minimum_at = -1;
  signed4 fits = 0;
  signed4 total_fits = peaks*49;
  for (signed4 peak = 0 ; peak < peaks && !stop_signal; peak ++)
    {
      if (peak_bpm[peak]==0) continue;
      unsigned4 a = (signed4)((float8)(audiorate*60*4)/(peak_bpm[peak]+0.5));
      unsigned4 b = (signed4)((float8)(audiorate*60*4)/(peak_bpm[peak]-0.5)); 
      if (a<startshift) a = startshift;
      if (a>stopshift) a = stopshift;
      if (b<startshift) b = startshift;
      if (b>stopshift) b = stopshift;
      assert(a<b);
      unsigned4 s = (b-a) / 25;  // 25 tries voor elke peak
      unsigned4 minimum = match[a-startshift] = phasefit(a, global_minimum);
      unsigned4 minimum_at = a;
      unsigned4 m;
      for (unsigned4 i = a ; i < b  && ! stop_signal; i += s)
	{
	  fits++;
	  if (match[i-startshift]) continue;
	  m = phasefit(i, global_minimum);
	  match[i-startshift] = m;
	  if (m>maximum) maximum = m;
	  if (m<minimum)
	    {
	      minimum = m;
	      minimum_at = i;
	      if (minimum < global_minimum || global_minimum_at == -1)
		{
		  global_minimum = minimum;
		  global_minimum_at = minimum_at;
		}
	    }
	  updateProcessingProgress(fits*100/total_fits);
	}
      peak_fit[peak]=minimum_at;
    }

  if (stop_signal)
    {
      status("Canceled during autocorrelation");
      return;
    }
  
  // now we have all peaks, it is time to do a fine scan of the missing 
  // information...
  for (signed4 peak = 0 ; peak < peaks; peak ++)
    {
      if (peak_fit[peak]==0) continue;
      unsigned4 a = peak_fit[peak]-12;
      unsigned4 b = peak_fit[peak]+12;
      if (a<startshift) a = startshift;
      if (a>stopshift) a = stopshift;
      if (b<startshift) b = startshift;
      if (b>stopshift) b = stopshift;
      assert(a<b);
      unsigned4 minimum = match[peak_fit[peak]-startshift];
      unsigned4 m = minimum;
      unsigned4 minimum_at = peak_fit[peak];
      for (unsigned4 i = a ; i < b ; i ++)
	{
	  fits++;
	  if (match[i-startshift]) continue;
	  m = phasefit(i, global_minimum);
	  match[i-startshift] = m;
	  if (m>maximum) maximum = m;
	  if (m<minimum)
	    {
	      minimum = m;
	      minimum_at = i;
	      if (minimum < global_minimum || global_minimum_at == -1)
		{
		  global_minimum = minimum;
		  global_minimum_at = minimum_at;
		}
	    }
	  updateProcessingProgress(fits*100/total_fits);
	}
      peak_fit[peak]=minimum_at;
    }

  if (stop_signal)
    {
      status("Canceled while autocorrelating");
      return;
    }

  // update tempo information 
  processing_progress = 100;
  status("Ready");
  
  set_measured_period("Peak Scanning",global_minimum_at);

  QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  fft_draw(p,IMAGE_XS,IMAGE_YS, shifter,1.0);
  signed4 X,Y;
  p.setPen(Qt::green);
  p.drawLine(IMAGE_XS/2,0,IMAGE_XS/2,IMAGE_YS-1);
  p.drawLine(IMAGE_XS/4,0,IMAGE_XS/4,IMAGE_YS-1);
  p.drawLine(IMAGE_XS*3/4,0,IMAGE_XS*3/4,IMAGE_YS-1);
  p.setPen(Qt::red);
  
  float8 bpm = (float8)(4*60*audiorate)/(float8)global_minimum_at;
  X = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
  p.drawLine(X,0,X,IMAGE_YS-1);
  p.setPen(Qt::blue);
  for(unsigned4 i = startshift ; i < stopshift ; i++ )
    {
      unsigned4 fout=match[i-startshift];
      if (fout)
	{
	  fout -= global_minimum;
	  bpm = (float8)(4*60*audiorate)/(float8)i;
	  X = (signed4)((float8)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  Y = (signed4)((signed8)fout * (signed8)r.height() / 
			(signed8) (maximum-global_minimum));
	  Y = r.height() - Y;
	  p.drawPoint(X,Y);
	  p.drawPoint(X+1,Y);
	  p.drawPoint(X,Y+1);
	  p.drawPoint(X+1,Y+1);
	}
    }
  p.end();
  BpmPix->setPixmap(*pm);
}

void BpmAnalyzerDialog::stopped()
{
  Analyzer::stopped();
  StartStop->setText("&Start");
  BusyToggle->toggle();
}

void BpmAnalyzerDialog::startStopButton()
{
  toggle();
}

void BpmAnalyzerDialog::started()
{
  StartStop->setText("&Stop");
  BusyToggle->toggle();
  Analyzer::started();
}

void BpmAnalyzerDialog::stop()
{
  status("Canceling...");
  Analyzer::stop();
}

void BpmAnalyzerDialog::finish()
{
  Analyzer::stop();
}

void BpmAnalyzerDialog::reset()
{
  tapcount = 0;
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::tap()
{
  if (++tapcount==1) {
    starttime=times(NULL);
    ::metronome->become_master();
  }
  else
    {
      /**
       * we have to decrease tapcount with one because at tick 2 the 
       * time passed counts for only one beat.
       */
      signed8 p = (((signed8)times(NULL)-(signed8)starttime)/(tapcount-1));
      p *= normalperiod;
      p /= currentperiod;
      p /= SkipBox->value();
      p *= 11025*4;
      p /= CLOCK_FREQ;
      
      /**
       * This is tricky. Because we modify the native tempo
       * the player will start playing faster because it tries 
       * to honour the current requested tempo (given by currentperiod). 
       * However, since we are tapping the tempo it is unwanted 
       * that the playing speed changes. Hence, we need to reset
       * the currentperiod so that it plays equally fast after the 
       * change in the native tempo.
       */
      signed8 oldcurrentperiod=currentperiod;
      signed8 oldnormalperiod=normalperiod;
      set_measured_period("Tap measurement",period_to_quad(p),false);
      ::metronome->changetempo(normalperiod*oldcurrentperiod/oldnormalperiod);
      
      tempo_type tempo = playing->get_tempo();
      setBpmBounds(tempo.lower(10),tempo.higher(10));
    }
  TapLcd->display(tapcount);
}
#endif // __loaded__bpm_analyzer_cpp__
