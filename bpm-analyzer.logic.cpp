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
#include <math.h>
#include "bpm-analyzer.logic.h"
#include "sys/times.h"
#include "version.h"
#include "player-core.h"
#include "fourier.h"
#include "scripts.h"

//#define PROFILER

#define IMAGE_XS 640
#define IMAGE_YS 480
const int shifter = 12;
const int spectrum_shifter = 2;

#ifdef PROFILER
long running_time;
clock_t last_start;
void start_timer()
{
  running_time = 0 ;
  last_start = times(NULL);
}

void pause_timer()
{
  running_time += times(NULL) - last_start;
}

void unpause_timer()
{
  last_start = times(NULL);
}

void stop_timer()
{
  running_time += times(NULL) - last_start;
  running_time/=CLOCK_FREQ;
}
#else
#define start_timer()
#define pause_timer()
#define unpause_timer()
#define stop_timer()
#endif


void fft(unsigned windowsize, 
	 bool reverse, 
	 fft_type * audio, 
	 fft_type * audioi, 
	 fft_type * freq,
	 fft_type *freqi)
{
  fft_double(windowsize,reverse,audio,audioi,freq,freqi);
}

BpmAnalyzerDialog::BpmAnalyzerDialog(SongPlayer*parent, const char*name, bool modal, WFlags f) :
  CountDialog(0,name,modal,f),
  ThreadedAnalyzer()
{
  player=parent;
  tapcount=0;
  audio=NULL;
  audiorate = 11025;
  startbpm = 120;
  stopbpm = 160;
  startshift = 0;
  stopshift = 0;
  playing->set_bpmcount_from(0);
  playing->set_bpmcount_to(100);
  bufsiz = 32 * 1024;
  freq = NULL;
  // internal communicative variables
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

//#define TEST
unsigned long BpmAnalyzerDialog::phasefit_diff(unsigned long i)
{
  unsigned long c;
  unsigned long mismatch=0;
#ifdef TEST
  for(c=i;c<audiosize;c++)
    mismatch+=abs((long)abs(audio[c])-(long)abs(audio[c-i]));
#else
  for(c=i;c<audiosize;c++)
    mismatch+=abs((long)audio[c]-(long)audio[c-i]);
#endif
  return mismatch;
}


unsigned long BpmAnalyzerDialog::phasefit_mult(unsigned long i)
{
#ifndef TEST
  unsigned long c;       
  unsigned long mismatch=0;
  for(c=i;c<audiosize;c++)
    mismatch+=((long)audio[c]*(long)audio[c-i])/4;
  return mismatch;
#else
  unsigned long c;       
  unsigned long mismatch=0x3fffffff;
  for(c=i;c<audiosize;c++)
    mismatch-=((long)audio[c]*(long)audio[c-i])/16;
  return mismatch;
#endif
}

unsigned long BpmAnalyzerDialog::phasefit(unsigned long i)
{
#ifdef TEST
  return phasefit_mult(i);
#else
  return phasefit_diff(i);
#endif
}


unsigned long BpmAnalyzerDialog::phasefit(unsigned long i, unsigned long clip)
{
  unsigned long c;
  unsigned long mismatch=0;
  if (!clip) return phasefit(i);
  assert(i<audiosize);
  for(c=i;c<audiosize && mismatch <= clip ;c++)
    mismatch+=abs((long)audio[c]-(long)audio[c-i]);
  return mismatch;
}


void BpmAnalyzerDialog::setBpmBounds(long start, long stop)
{
   char tmp[500];
   char tmp2[500];
   startbpm=start;
   stopbpm=stop;
   sprintf(tmp,"%d",(int)startbpm);
   FromBpmEdit->setText(tmp);
   sprintf(tmp2,"%d",(int)stopbpm);
   ToBpmEdit->setText(tmp2);
}

void BpmAnalyzerDialog::setPercentBounds(long startper, long stopper)
{
   char tmp[500];
   char tmp2[500];
   // de bpmcount_from en bpm_count_to fields moeten 
   // ingevuld worden
   playing->set_bpmcount_from(startper);
   playing->set_bpmcount_to(stopper);
   sprintf(tmp,"%d",playing->get_bpmcount_from());
   FromPercentEdit->setText(tmp);
   sprintf(tmp2,"%d",playing->get_bpmcount_to());
   ToPercentEdit->setText(tmp2);
}

void BpmAnalyzerDialog::timerTick()
{
  ReadingBar->setProgress(reading_progress);
  CountingBar->setProgress(processing_progress);
}

void BpmAnalyzerDialog::readAudio()
{
  FILE * raw = openRawFile(playing,arg_rawpath);
  
  // read complete file shrunken down into memory
  audiosize=fsize(raw);
  long startpercent=(long)((long long)audiosize*(long long)playing->get_bpmcount_from()/(long long)100);
  startpercent-=startpercent%4;
  long stoppercent=(long)((long long)audiosize*(long long)playing->get_bpmcount_to()/(long long)100);
  stoppercent-=stoppercent%4;
  audiosize=stoppercent-startpercent;
  audiosize/=(4*(WAVRATE/audiorate));
  audio=allocate(audiosize+1,unsigned char);
  
  // reading in memory
  StatusLabel->setText("Reading");
  unsigned long pos=0;
  long count, redux, i;
  signed short buffer[bufsiz];
  fseek(raw,startpercent,SEEK_SET);
  while(pos<audiosize && !stop_signal)
    {
      // we read in bufsize samples 
      // als hem 1000 zegt hebben we dus eigenlijk 2000 samples binnen gekregen
      count=readsamples((unsigned4*)buffer,bufsiz/2,raw);
      reading_progress = pos*100/audiosize;
      for (i = 0 ; i < count * 2 ; i += 2 * (WAVRATE/audiorate) )
	{
	  signed long int left, right,mean;
#ifndef TEST
	  left=abs(buffer[i]);
	  right=abs(buffer[i+1]);
	  mean=(left+right)/2;
	  redux=abs(mean)/128;
#else
	  left=buffer[i];
	  right=buffer[i+1];
	  mean=(left+right)/2;
	  redux=mean/128;
#endif
	  if (pos+i/(2*(WAVRATE/audiorate))>=audiosize) break;
	  assert(pos+i/(2*(WAVRATE/audiorate))<audiosize);
	  audio[pos+i/(2*(WAVRATE/audiorate))]=(unsigned char)redux;
	}
      pos+=count/(WAVRATE/audiorate);
    }
  fclose(raw);
  if (stop_signal)
    {
      deallocate(audio);
      audio=NULL;
      audiosize=0;
      StatusLabel->setText("Canceled while reading");
      return;
    }
  reading_progress = 100;
}

void BpmAnalyzerDialog::readAudioBlock(int blocksize)
{
  FILE * raw = openRawFile(playing,arg_rawpath);
  
  // read complete file shrunken down into memory
  audiosize=fsize(raw);  // uitgedrukt in bytes...
  long startpercent=(long)((long long)audiosize*(long long)playing->get_bpmcount_from()/(long long)100);
  startpercent/=4;       // uitgedrukt in sample dus...
  long stoppercent=(long)((long long)audiosize*(long long)playing->get_bpmcount_to()/(long long)100);
  stoppercent/=4;        // uitgedrukt in samples
  audiosize = stoppercent-startpercent;  // uitgedrukt in samples
  audiosize /= blocksize;        // uitgedrukt in blokken
  audio = allocate( audiosize, unsigned char );
  
  // reading in memory
  processing_progress = 0;
  StatusLabel->setText("Reading audio (blocksize "+QString::number(blocksize)+")");
  unsigned long pos = 0;
  long count;
  longtrick buffer[blocksize];
  fseek(raw,startpercent*4,SEEK_SET);
  while(pos<audiosize && !stop_signal)
    {
      count = 0;
      do 
	{
	  count = readsamples(buffer+count,blocksize-count,raw);
	}
      while (count < blocksize );
      reading_progress = pos*100/audiosize;
      unsigned long total = 0;
      for (int i = 0 ; i < blocksize ; i ++ )
	{
	  signed long int left, right;
	  left=abs(buffer[i].leftright.left);
	  right=abs(buffer[i].leftright.right);
	  total+=(left+right)/256;
	}
      audio[pos]=total/blocksize;
      pos++;
    }
  fclose(raw);
  
  if (stop_signal)
    {
      deallocate(audio);
      audio=NULL;
      audiosize=0;
      StatusLabel->setText("Canceled while reading");
      return;
    }
  
  reading_progress = 100;
}

void BpmAnalyzerDialog::rangeCheck()
  {
    unsigned long int val;
    bool changed = false;
    val = atoi(FromPercentEdit->text());
    if (changed |= (unsigned)playing->get_bpmcount_from() != val) 
      playing->set_bpmcount_from(val);
    val = atoi(ToPercentEdit->text());
    if (changed |= (unsigned)playing->get_bpmcount_to() != val) 
      playing->set_bpmcount_to(val);
    val = atoi(FromBpmEdit->text());
    if (startbpm != val)
      startbpm=val;
    val = atoi(ToBpmEdit->text());
    if (stopbpm != val)
      stopbpm=val;
    if (changed && audio)
      {
	deallocate(audio);
	audio=NULL;
	audiosize=0;
      }
  }

fft_type index2autocortempo(int i)
{
  assert(i);
  fft_type measure_period_in_ticks = i<<spectrum_shifter;
  fft_type measure_period_in_secs  = measure_period_in_ticks/(fft_type)WAVRATE;
  fft_type measure_frequency_in_hz = 1/measure_period_in_secs;
  fft_type measure_frequency_in_bpm = measure_frequency_in_hz*60.0;
  fft_type beat_frequency_in_bpm = measure_frequency_in_bpm*4.0;
  return beat_frequency_in_bpm;
}

void BpmAnalyzerDialog::autocorrelate_draw(QPainter &p, int xs, int ys, int shifter)
{
  if (!freq) return;
  p.setPen(Qt::gray);
  
  // first find the upper and lower bounds of the energy spectrum
  double min_energy=1.0;
  double max_energy=0.0;
  for(int i = 0 ; i < windowsize/2 ; i ++)
    {
      if (freq[i]>max_energy)
	max_energy=freq[i];
      if (freq[i]<min_energy)
	min_energy=freq[i];
    }
  max_energy-=min_energy;

  // then draw it using these bounds
  int lastx = 0, lasty = 0, lastcount = 0;
  for(int i = 1 ; i <windowsize/2; i ++)
    {
      fft_type bpm = index2autocortempo(i);      
      // calculate position
      int x = (int)((double)xs*(bpm-startbpm)/(stopbpm-startbpm));
      int y = (int)((double)ys-(double)ys*freq[i]);
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
  
  // now draw all the peaks..
  for (int i = 0 ; i < peaks ; i ++)
    {
      // jus for the fun of it, choose other colors...
      QColor c(255-255*i/peaks,0,0);
      p.setPen(c);
      double bpm = peak_bpm[i];
      double energy = peak_energy[i];
      int x = (int)((double)xs*(bpm-startbpm)/(stopbpm-startbpm));
      int y = (int)((double)ys-(double)ys*energy);
      QString text = QString::number(i)+") "+QString::number(bpm);
      p.drawText(x,y,text);
    }
}

void BpmAnalyzerDialog::fft_draw(QPainter &p, int xs, int ys, int shifter, double bpm_divisor)
{
  if (!freq) return;
  p.setPen(Qt::gray);
  
  // first find the upper and lower bounds of the energy spectrum
  double min_energy=1.0;
  double max_energy=0.0;
  for(int i = 0 ; i < windowsize/2 ; i ++)
    {
      if (freq[i]>max_energy)
	max_energy=freq[i];
      if (freq[i]<min_energy)
	min_energy=freq[i];
    }
  max_energy-=min_energy;

  // then draw it using these bounds
  int lastx = 0, lasty = 0, lastcount = 0;
  for(int i = 0 ; i <windowsize/2; i ++)
    {
      // calculate bpm
      double bpm = Index_to_frequency(windowsize,i);  // relatief tov WAVRATE
      bpm*=(double)WAVRATE;                           // in Hz tov non collapsed WAVRATE
      for(int j = 0 ; j < shifter; j ++) bpm/=2.0;    // in collapsed WAVRATE
      bpm*=60.0/bpm_divisor;                                      // in BPM.
      // calculate position
      int x = (int)((double)xs*(bpm-startbpm)/(stopbpm-startbpm));
      int y = (int)((double)ys-(double)ys*freq[i]);
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
  
  // now draw all the peaks..
  for (int i = 0 ; i < peaks ; i ++)
    {
      // jus for the fun of it, choose other colors...
      QColor c(255-255*i/peaks,0,0);
      p.setPen(c);
      double bpm = peak_bpm[i];
      double energy = peak_energy[i];
      int x = (int)((double)xs*(bpm-startbpm)/(stopbpm-startbpm));
      int y = (int)((double)ys-(double)ys*energy);
      QString text = QString::number(i)+") "+QString::number(bpm);
      p.drawText(x,y,text);
    }
}

void BpmAnalyzerDialog::fft()
{
  // first we need to read all data in memory en scale it down
  signed8 audiosize;
  FILE * raw = openRawFile(playing,arg_rawpath);
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // owkee, nu moeten we dat delen door 2^shifter dan brengt ons dat binnen range van 322.9 BPM
  audiosize>>=shifter;
  signed4 blocksize = 1 << shifter;
  longtrick *block = allocate(blocksize,longtrick);
  double *audio = allocate(audiosize,double);
  for(int i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (int j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].leftright.left);
      // sum+=(long)block[j].leftright.left*(long)block[j].leftright.left;
      sum/=blocksize;
      audio[i]=sum;
    }
  
  // FFT
  windowsize = 1;
  for(int tmp = audiosize; tmp; tmp>>=1, windowsize<<=1) ;
  if (windowsize>audiosize) windowsize>>=1;

  // printf ("audiosize = %d\n",(int) audiosize);
  // printf ("windowsize = %d\n",(int) windowsize);
  // printf ("blocksize = %d\n",(int) blocksize);
  
  freq = allocate(windowsize,fft_type);
  fft_type *freqi = allocate(windowsize,fft_type);
  ::fft(windowsize,false,audio,NULL,freq,freqi);

  // rescale the entire thing
  double max = 0;
  for(int i = 0 ; i <windowsize/2 ; i ++)
    {
      freq[i]=10.0*log(fabs(freq[i]));
      if (freq[i]>max) max=freq[i];
    }
  for (int i = 0 ; i < windowsize / 2 ; i ++)
    freq[i]/=max;
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = allocate(peaks, fft_type);
  peak_energy = allocate(peaks, fft_type);
  fft_type *copy = allocate(windowsize / 2, fft_type);
  for(int i = 0 ; i < windowsize/2 ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // bpm left and right...
  
  for(int j = 0 ; j < peaks ; j ++)
    {
      double energy = 0;
      double at = 0;
      for(int i = 0 ; i <windowsize/2 ; i ++)
	{
	  // obtain bpm
	  double bpm = Index_to_frequency(windowsize,i);         // uitgedruk relatief tov WAVRATE
	  bpm*=(double)WAVRATE;                           // uitgedrukt in Hz tov non collapsed WAVRATE
	  for(int j = 0 ; j < shifter; j ++) bpm/=2.0;    // uitgedrukt in collapsed WAVRATE
	  bpm*=60.0;                                      // uitgedrukt in BPM.
	  // skip or break ?
	  if (bpm<startbpm) continue;
	  if (bpm>stopbpm) break;
	  // is larger than any of the nown peaks ?
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
      for(int i = 0 ; i <windowsize/2 ; i ++)
	{
	  // obtain bpm
	  double bpm = Index_to_frequency(windowsize,i);         // uitgedruk relatief tov WAVRATE
	  bpm*=(double)WAVRATE;                           // uitgedrukt in Hz tov non collapsed WAVRATE
	  for(int j = 0 ; j < shifter; j ++) bpm/=2.0;    // uitgedrukt in collapsed WAVRATE
	  bpm*=60.0;                                      // uitgedrukt in BPM.
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  
  deallocate(freqi);
  deallocate(audio);
}

void BpmAnalyzerDialog::enveloppe_spectrum()
{
  // first we need to read all data in memory en scale it down
  start_timer();
  signed8 audiosize;
  FILE * raw = openRawFile(playing,arg_rawpath);
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // owkee, nu moeten we dat delen door 2^spectrum_shifter
  audiosize>>=spectrum_shifter;
  signed4 blocksize = 1 << spectrum_shifter;
  longtrick *block = allocate(blocksize,longtrick);
  fft_type *audio = allocate(audiosize,fft_type);
  for(int i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (int j = 0 ; j < blocksize ; j ++)
	sum+=abs(block[j].leftright.left);
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("Audio has been read...\n");
  
  // obtain FFT windowsize
  // FFT with a windowsize smaller than the maximum length
  windowsize = 1;
  for(int tmp = audiosize; tmp; tmp>>=1, windowsize<<=1) ;
  if (windowsize>audiosize) windowsize>>=1;
  
  freq = allocate(windowsize,fft_type);
  fft_type *freqi = allocate(windowsize,fft_type);
  ::fft(windowsize,false,audio,NULL,freq,freqi);
  printf("FFT has been done\n");

  // rescale the entire thing
  fft_type max = 0;
  fft_type min = -1.0;
  for(int i = 0 ; i <windowsize/2 ; i ++)
    {
      fft_type bpm = Index_to_frequency(windowsize,i); // tov WAVRATE
      bpm*=(fft_type)WAVRATE;                          // in Hz tov non collapsed WAVRATE
      for(int j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;    // uitgedrukt in collapsed WAVRATE
      bpm*=60.0;                                      // uitgedrukt in BPM.
      if (bpm<startbpm) continue;
      if (bpm>stopbpm) break;
      freq[i]=10.0*log(fabs(freq[i]));
      // freq[i]=fabs(freq[i]);
      if (freq[i]>max) max=freq[i];
      if (freq[i]<min || min < 0.0) min=freq[i];
    }
  printf("maximum =%g, minimum = %g\n",max,min);
  for (int i = 0 ; i < windowsize / 2 ; i ++)
    freq[i]=(freq[i]-min)/(max-min);
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = allocate(peaks, fft_type);
  peak_energy = allocate(peaks, fft_type);
  fft_type *copy = allocate(windowsize / 2, fft_type);
  for(int i = 0 ; i < windowsize/2 ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // bpm left and right...
  
  for(int j = 0 ; j < peaks ; j ++)
    {
      fft_type  energy = 0, at = 0;
      for(int i = 0 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = Index_to_frequency(windowsize,i); // tov WAVRATE
	  bpm*=(fft_type)WAVRATE;                          // in Hz tov non collapsed WAVRATE
	  for(int j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;    // uitgedrukt in collapsed WAVRATE
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
	{
	  stop_timer();
	  set_measured_period("Enveloppe",(int)(4.0*11025.0*60.0*4.0/at));
	}
      
      // clear neighbors
      for(int i = 0 ; i <windowsize/2 ; i ++)
	{
	  // obtain bpm
	  fft_type bpm = Index_to_frequency(windowsize,i); // relatief tov WAVRATE
	  bpm*=(double)WAVRATE;                           // in Hz tov non collapsed WAVRATE
	  for(int j = 0 ; j < spectrum_shifter; j ++) bpm/=2.0;    // in collapsed WAVRATE
	  bpm*=60.0;                                      // in BPM.
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm>at+range) break;
	}
    }
  
  deallocate(freqi);
  deallocate(audio);
}

void BpmAnalyzerDialog::autocorrelate_spectrum()
{
  // 0. read everything in memory
  start_timer();
  signed8 audiosize;
  FILE * raw = openRawFile(playing,arg_rawpath);
  fseek(raw,0,SEEK_END);
  audiosize = ftell(raw) / 4;
  fseek(raw,0,SEEK_SET);
  // owkee, nu moeten we dat delen door 2^spectrum_shifter
  audiosize>>=spectrum_shifter;
  signed4 blocksize = 1 << spectrum_shifter;
  longtrick *block = allocate(blocksize,longtrick);
  fft_type *audio = allocate(audiosize,fft_type);
  for(int i = 0 ; i < audiosize; i++)
    {
      signed8 sum = 0;
      signed4 read = 0;
      while(read<blocksize)
	read+=readsamples(block+read,blocksize-read,raw);
      for (int j = 0 ; j < blocksize ; j ++)
	sum+=block[j].leftright.left;
      sum/=blocksize;
      audio[i]=sum;
    }
  printf("Audio has been read...\n");
  
  // 1. do a FFT of the entire sample
  // obtain FFT windowsize
  // FFT with a windowsize smaller than the maximum length
  windowsize = 1;
  for(int tmp = audiosize; tmp; tmp>>=1, windowsize<<=1) ;
  if (windowsize>audiosize) windowsize>>=1;
  
  fft_type *freq_tmp  = allocate(windowsize,fft_type);
  fft_type *freqi_tmp = allocate(windowsize,fft_type);
  ::fft(windowsize,false,audio,NULL,freq_tmp,freqi_tmp);
  deallocate(audio);
  printf("Forward FFT has been done\n");

  // 2. modify freq[i]=abs(freq[i]);
  for(int i = 0 ; i < windowsize; i ++)
    freq_tmp[i] = freq_tmp[i] * freq_tmp[i]
      + freqi_tmp[i] * freqi_tmp[i];
  printf("Copy has been made\n");
  
  // 3. do an inverse fourier transform of freq[i]
  freq  = allocate(windowsize,fft_type);
  ::fft(windowsize,true,freq_tmp,NULL,freq,freqi_tmp);
  
  deallocate(freq_tmp);
  deallocate(freqi_tmp);
  printf("Backward FFT has been done\n");

  // 4. rescale & find peaks 
  // rescale the entire thing
  fft_type max = 0;
  fft_type min = -1.0;
  for(int i = 1 ; i <windowsize/2 ; i ++)
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
  for (int i = 0 ; i < windowsize / 2 ; i ++)
    freq[i]=(freq[i]-min)/(max-min);
  
  // detect peak bpm's
  peaks = 10;
  peak_bpm = allocate(peaks, fft_type);
  peak_energy = allocate(peaks, fft_type);
  fft_type *copy = allocate(windowsize / 2, fft_type);
  for(int i = 0 ; i < windowsize/2 ; i++) copy[i]=freq[i];
  fft_type range = 0.5; // bpm left and right...
  
  for(int j = 0 ; j < peaks ; j ++)
    {
      fft_type  energy = 0, at = 0;
      for(int i = 1 ; i <windowsize/2 ; i ++)
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
	{
	  stop_timer();
	  set_measured_period("Autocorrelation",(int)(4.0*11025.0*60.0*4.0/at));
	}
      printf("Peak %d at %g with strength %g\n",j,at,energy);
      
      // clear neighbors
      for(int i = 1 ; i <windowsize/2 ; i ++)
	{
	  fft_type bpm = index2autocortempo(i);
	  if (bpm>=at-range && bpm<=at+range)
	    copy[i]=0;
	  if (bpm<at-range) break;
	}
    }
}

// momenteel wordt een scan 457 keer uitgevoerd... ongeveer...
void BpmAnalyzerDialog::run()
{
  // check wether ranges are changed
  rangeCheck();
  
  // set labels...
  char d[500];
  sprintf(d,"%2g",(double)startbpm);
  X0->setText(d);
  sprintf(d,"%2g",(double)startbpm+3.0*(double)(stopbpm-startbpm)/4.0);
  X3->setText(d);
  sprintf(d,"%2g",(double)startbpm+(double)(stopbpm-startbpm)/2.0);
  X2->setText(d);
  sprintf(d,"%2g",(double)startbpm+(double)(stopbpm-startbpm)/4.0);
  X1->setText(d);
  sprintf(d,"%2g",(double)stopbpm);
  X4->setText(d);
  

#ifdef PROFILER
  printf("songtime : %s : title : %s[%s]%s\n",
	 (const char*)(playing->get_time()),
	 (const char*)(playing->get_display_title()),
	 (const char*)(playing->get_display_author()),
	 (const char*)(playing->get_display_version()));
  set_measured_period("Manually",normalperiod);
  
  rayshoot_scan();
  autocorrelate_spectrum();
  enveloppe_spectrum();
  return;
#endif
  
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
  const unsigned blockshifter_max = 8;  // this is a good start, goes fast and is accurately enough to skip no import points
  unsigned blockshifter = blockshifter_max;
  signed* mismatch_array[blockshifter_max+1];
  unsigned mean[blockshifter_max+1];
  unsigned minimum_at = 0;
  unsigned minima[blockshifter_max+1];
  for (unsigned i = 0 ; i <= blockshifter_max; i++) mean[i]=0;
  unsigned maxima[blockshifter_max+1];
#ifdef PROFILER
  start_timer();
  unsigned rays_shot[blockshifter_max+1];
  for (int i = 0 ; i <= blockshifter_max; i++) rays_shot[i]=0;
#endif
  // depending on the audiorate we should stop at a certain blockshift minimum...
  unsigned blockshifter_min; 
  if (audiorate == 44100) blockshifter_min = 0;
  else if (audiorate == 22050) blockshifter_min = 1;
  else if (audiorate == 11025) blockshifter_min = 2;
  else assert(0);
  stopshift = 44100 * 60 * 4 / startbpm ;
  startshift = 44100 * 60 * 4 / stopbpm ;
  for( unsigned4 i = 0; i <= blockshifter_max ; i ++)
    {
      mismatch_array[i] = allocate(stopshift-startshift, signed);
      for(unsigned4 j = 0 ; j < stopshift-startshift ; j ++)
	mismatch_array[i][j] = -1;
    }
  while(blockshifter>=blockshifter_min && ! stop_signal)
    {
      unsigned long blocksize = 1 << blockshifter;
      // first read audio
      readAudioBlock(blocksize);
      // calculate all mismatches
      signed * mismatch = mismatch_array[blockshifter] - startshift;
      signed * prev_mismatch = NULL;
      unsigned prev_maximum = 0;
      if (blockshifter < blockshifter_max)
	{
	  prev_mismatch = mismatch_array[blockshifter+1] - startshift;
	  // the mismatch array goes always from 0 to stophift-startshift-1
	  // the prev_mismatch  hence goes from  startshift to stopshift - 1
	  prev_maximum = mean[blockshifter+1]; 
	}
      StatusLabel->setText("Scanning "+QString::number(blockshifter_max-blockshifter)
			   +"/" + QString::number(blockshifter_max-blockshifter_min));
      if (!prev_mismatch)
	for (unsigned i = startshift ; i < stopshift && ! stop_signal; i ++ )
	  {
	    unsigned phase = i >> blockshifter;
	    unsigned store = phase << blockshifter;
	    processing_progress = (i - startshift) *100 / (stopshift - startshift);
	    if (store!=i) continue;
	    unsigned m = phasefit(phase);
#ifdef PROFILER
	    rays_shot[blockshifter]++;
#endif
	    mismatch[store]=m;
	  }
      else
	{
	  // first a dry run to see how many times we would execute the phasefit;
	  int phasefit_called = 0;
	  int phasefit_total = 0;
	  for (unsigned i = startshift ; i < stopshift && ! stop_signal; i++ )
	    {
	      unsigned phase = i >> blockshifter;
	      unsigned store = phase << blockshifter;
	      if (store!=i) continue;
	      unsigned prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (next_store >= stopshift ) break;
	      unsigned prev_val = prev_mismatch[prev_store]; // sign is important !
	      unsigned next_val = prev_mismatch[next_store]; // sign is important !
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		phasefit_total++;
	    }
#ifdef PROFILER
	  rays_shot[blockshifter]=phasefit_total;
#endif
	  // now wet run..
	  for (unsigned i = startshift ; i < stopshift && ! stop_signal; i++ )
	    {
	      unsigned phase = i >> blockshifter;
	      unsigned store = phase << blockshifter;
	      processing_progress = phasefit_called*100/phasefit_total;
	      if (store!=i) continue;
	      // we hebben een positie die een macht van 2 is...
	      // dus de vorige was op - en + blocksize *2
	      unsigned prev_store = ((phase / 2) * 2) << blockshifter;
	      unsigned next_store = (((phase / 2) + 1 ) * 2) << blockshifter;
	      if (next_store >= stopshift ) break;
	      unsigned prev_val = prev_mismatch[prev_store]; // sign is important !
	      unsigned next_val = prev_mismatch[next_store]; // sign is important !
	      if (prev_val < prev_maximum || next_val < prev_maximum)
		{
		  unsigned m = phasefit(phase);  // we moeten de phase kleiner maken omdat de array geresampled is op blocksize
		  mismatch[store]=m;
		  phasefit_called++;
		}
	    }
	}
      processing_progress = 100;
      // find minimum, translate, maximum, mean
      signed minimum = mismatch[startshift];
      minimum_at = startshift;
      for (unsigned i = startshift ; i < stopshift ; i ++)
	if (minimum == -1 || (mismatch[i] >= 0 && mismatch[i] < minimum))
	  minimum = mismatch[ minimum_at = i ];
      minima[blockshifter] = minimum_at;
      for (unsigned i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0)
	  mismatch[i]-=minimum;
      signed maximum = -1;
      for (unsigned i = startshift ; i < stopshift ; i ++)
	if (mismatch[i] >= 0 && (mismatch [i] > maximum || maximum < 0)) 
	  maximum = mismatch[i];
      long long total = 0;
      int count = 0;
      for (unsigned i = startshift ; i < stopshift ; i ++)
	if (mismatch[i]>=0)
	  {
	    count++;
	    total+=mismatch[i];
	  }
      maxima[blockshifter]=maximum;
      mean[blockshifter]=total/count;
      
      // draw the sucker
      pause_timer();
      QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
      QPainter p;
      p.begin(pm);
      QRect r(QRect(0,0,pm->width(),pm->height()));
      p.fillRect(r,Qt::white);
      p.setPen(Qt::gray);
      p.drawLine(IMAGE_XS/2,0,IMAGE_XS/2,IMAGE_YS);
      p.drawLine(IMAGE_XS/4,0,IMAGE_XS/4,IMAGE_YS);
      p.drawLine(IMAGE_XS*3/4,0,IMAGE_XS*3/4,IMAGE_YS);
      
      // draw points
      for (unsigned i = blockshifter_max ; i >= blockshifter ; i --)
	{
	  unsigned slice_maximum = maxima[i];
	  unsigned pos = r.height() - ((long long)mean[i] * (long long) r.height() / (long long)slice_maximum);
	  QColor c(0,0,0);
	  int kleur = (i - blockshifter_min) * 128 / (blockshifter_max - blockshifter_min);
	  c.setHsv(kleur, 255, 255);
	  p.setPen(c);
	  p.drawLine(0,pos,r.width(),pos);
	  double bpm = (double)(4.0*60.0*44100.0)/(double)minima[i];
	  pos = (int)((double)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  p.drawLine(pos,0,pos,r.height());
	  	  
	  for( unsigned j = startshift ; j < stopshift ; j ++ )
	    {
	      signed long long y = mismatch_array[i][j-startshift];
	      if (y > 0)
		{
		  y *= (long long) r.height();
		  y /= (long long) slice_maximum;
		  y = r.height() - y;
		  bpm = (double)(4.0*60.0*44100.0)/(double)j;
		  unsigned x = (int)((double)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
		  p.drawPoint(x,y);
		  p.drawPoint(x+1,y);
		  p.drawPoint(x,y+1);
		  p.drawPoint(x+1,y+1);
		}
	    }
	}
      p.end();
      BpmPix->setPixmap(*pm);
      unpause_timer();
      
      // next step
      blockshifter --;
    }

  stop_timer();
  
  if (stop_signal)
    {
      StatusLabel->setText("Canceled");
      return;
    }
  
  StatusLabel->setText("Ready");
  
#ifdef PROFILER
  float q = 0.0;
  for(int i = blockshifter_max ; i > blockshifter_min ; i--)
    q+=(double)rays_shot[i-1]/(double)rays_shot[i];
  q/=(double)(blockshifter_max-blockshifter_min+1);
  printf("Q: %g\n",q);
#endif
  set_measured_period("Rayshooting",minimum_at);
}

void BpmAnalyzerDialog::set_measured_period(QString technique, int p)
{
#ifdef PROFILER
  printf("technique : %s : result : %d : time : %d\n",
	 (const char*) technique, p, running_time, playing->get_time());
#else
  playing->set_period(p/4);
  normalperiod=playing->get_period()*4;
  currentperiod=normalperiod;
#endif
}

void BpmAnalyzerDialog::peak_scan()
{
  StatusLabel->setText("Finding least broken ray based on FFT");
  stopshift=audiorate*60*4/startbpm;
  startshift=audiorate*60*4/stopbpm;
  unsigned long match[stopshift-startshift];
  for (unsigned i = 0 ; i < stopshift - startshift ; i ++)
    match[i]=0;
  peak_fit = allocate(peaks, int);
  for (int i = 0 ; i < peaks ; i ++)
    peak_fit[i]=0;
  
  unsigned long int maximum = 0;
  unsigned long int global_minimum = 0;
  int global_minimum_at = -1;
  int fits = 0;
  int total_fits = peaks*49;
  for (int peak = 0 ; peak < peaks && !stop_signal; peak ++)
    {
      if (peak_bpm[peak]==0) continue;
      unsigned long a = (int)((double)(audiorate*60*4)/(peak_bpm[peak]+0.5));
      unsigned long b = (int)((double)(audiorate*60*4)/(peak_bpm[peak]-0.5)); 
      if (a<startshift) a = startshift;
      if (a>stopshift) a = stopshift;
      if (b<startshift) b = startshift;
      if (b>stopshift) b = stopshift;
      assert(a<b);
      unsigned long s = (b-a) / 25;  // 25 tries voor elke peak
      unsigned long minimum = match[a-startshift] = phasefit(a, global_minimum);
      unsigned long minimum_at = a;
      unsigned long m;
      for (unsigned long i = a ; i < b  && ! stop_signal; i += s)
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
	  // process meter is 
	  processing_progress = fits*100/total_fits;
	}
      peak_fit[peak]=minimum_at;
    }

  if (stop_signal)
    {
      StatusLabel->setText("Canceled while autocorrelating");
      return;
    }
  
  // now we have all peaks, it is time to do a fine scan of the missing information...
  for (int peak = 0 ; peak < peaks; peak ++)
    {
      if (peak_fit[peak]==0) continue;
      unsigned long a = peak_fit[peak]-12;
      unsigned long b = peak_fit[peak]+12;
      if (a<startshift) a = startshift;
      if (a>stopshift) a = stopshift;
      if (b<startshift) b = startshift;
      if (b>stopshift) b = stopshift;
      assert(a<b);
      unsigned long minimum = match[peak_fit[peak]-startshift];
      unsigned long m = minimum;
      unsigned long minimum_at = peak_fit[peak];
      for (unsigned long i = a ; i < b ; i ++)
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
	  // process meter is 
	  processing_progress = fits*100/total_fits;
	}
      peak_fit[peak]=minimum_at;
    }

  if (stop_signal)
    {
      StatusLabel->setText("Canceled while autocorrelating");
      return;
    }

  // update tempo information 
  processing_progress = 100;
  StatusLabel->setText("Ready");
  
  playing->set_period(global_minimum_at);
  normalperiod=playing->get_period()*4;
  currentperiod=normalperiod;
  if (WAVRATE==22050)
    {
      currentperiod/=2;
      normalperiod/=2;
    }
  
  // draw vertical lines at 1/4; 2/4; 3/4
  QPixmap *pm = new QPixmap(IMAGE_XS,IMAGE_YS);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  fft_draw(p,IMAGE_XS,IMAGE_YS, shifter,1.0);
  int X,Y;
  p.setPen(Qt::green);
  p.drawLine(IMAGE_XS/2,0,IMAGE_XS/2,IMAGE_YS-1);
  p.drawLine(IMAGE_XS/4,0,IMAGE_XS/4,IMAGE_YS-1);
  p.drawLine(IMAGE_XS*3/4,0,IMAGE_XS*3/4,IMAGE_YS-1);
  p.setPen(Qt::red);
  
  double bpm = (double)(4*60*audiorate)/(double)global_minimum_at;
  X = (int)((double)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
  p.drawLine(X,0,X,IMAGE_YS-1);
  
  // draw points
  p.setPen(Qt::blue);
  for(unsigned i = startshift ; i < stopshift ; i++ )
    {
      unsigned long fout=match[i-startshift];
      if (fout)
	{
	  fout -= global_minimum;
	  bpm = (double)(4*60*audiorate)/(double)i;
	  X = (int)((double)r.width()*(bpm-startbpm)/(stopbpm-startbpm));
	  Y = (int)((long long)fout * (long long)r.height() / (long long) (maximum-global_minimum));
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

void BpmAnalyzerDialog::stoppedAnalyzing()
{
  ThreadedAnalyzer::stoppedAnalyzing();
  StartButton->setText("&Start");
  BusyToggle->toggle();
}

void BpmAnalyzerDialog::startStopButton()
{
  startStopAnalyzer();
}

void BpmAnalyzerDialog::startAnalyzer()
{
  StartButton->setText("&Stop");
  BusyToggle->toggle();
  ThreadedAnalyzer::startAnalyzer();
}

void BpmAnalyzerDialog::stopAnalyzer()
{
  StatusLabel->setText("Canceling...");
  ThreadedAnalyzer::stopAnalyzer();
}

void BpmAnalyzerDialog::finish()
{
  ThreadedAnalyzer::finish();
  accept();
}

void BpmAnalyzerDialog::store()
{
}

void BpmAnalyzerDialog::reset()
{
  tapcount = 0;
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::tap()
{
  // increase tapcount
  tapcount++;
  if (tapcount==1)
    {
	starttime=times(NULL);
    }
  else
    {
      // we have to decrease tapcount with one because at tick 2 the 
      // time passed counts for only one beat.
      int p = (((signed8)times(NULL)-(signed8)starttime)/(tapcount-1));
      p /= SkipBox->value();
      p *= 11025*4;
      p /= CLOCK_FREQ;
      playing->set_period(p,false);
      normalperiod=playing->get_period()*4;
      currentperiod=normalperiod;
      if (WAVRATE==22050)
	{
	  currentperiod/=2;
	  normalperiod/=2;
	}
      double tempo = playing->get_tempo();
      setBpmBounds((long)(tempo-10.0),(long)(tempo+10.0));
    }
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::incBpm()
{
  int p = playing->get_period()-10;
  if(p < 100) p=100;
  playing->set_period(p);
  normalperiod=playing->get_period() * 4;
  currentperiod=normalperiod;
  if (WAVRATE==22050)
    {
      currentperiod/=2;
      normalperiod/=2;
    }
}

void BpmAnalyzerDialog::decBpm()
{
  int p = playing->get_period()+10;
  playing->set_period(p);
  normalperiod=playing->get_period() * 4;
  currentperiod=normalperiod;
  if (WAVRATE==22050)
    {
      currentperiod/=2;
      normalperiod/=2;
    }
}
