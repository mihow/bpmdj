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
#include <qcombobox.h>
#include <libgen.h>
#include <qslider.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <qmessagebox.h>
#include "pattern-analyzer.logic.h"
#include "spectrumanalyzer.logic.h"
#include "sys/times.h"
#include "kbpm-play.h"
#include "fourier.h"
#include "version.h"
#include "scripts.h"

#define COLLAPSE 4
#define sample_freq ((double)WAVRATE/(double)COLLAPSE)

PatternAnalyzerLogic::PatternAnalyzerLogic(bool showreaderprogress, SongPlayer*parent, const char*name, bool modal, WFlags f) :
  PatternDialog(0,name,modal,f)
{
  period=normalperiod;
  data = NULL;
  filter = NULL;
  filtersize = 0;
  readFile(showreaderprogress);
  show();
  showPattern();
}

void PatternAnalyzerLogic::readFile(bool showreaderprogress)
{
  if (data) return;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  // read in memory and shrink it 
  FILE * raw=openRawFile(playing,arg_rawpath);
  audiosize=fsize(raw)/4;
  if (audiosize == 0) return;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  data = allocate(audiosize/COLLAPSE,compressed);
  
  QProgressDialog * progress = NULL;
  if (showreaderprogress) 
    {
      progress = new QProgressDialog();
      progress->setTotalSteps(audiosize/bufsize);
      progress->show();
    }

  while(pos<audiosize)
    {
      long toread = audiosize - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      if (progress) progress->setProgress(pos/bufsize);
      app->processEvents();

      for(int i = 0 ; i<count/COLLAPSE; i++)
	{
	  unsigned4 sum=0;
	  for(int j = 0 ; j < COLLAPSE ; j++)
	    {
	      assert(i*COLLAPSE+j<count);
	      sum+=abs(buffer[i*COLLAPSE+j].leftright.left)/(1<<7);
	      sum+=abs(buffer[i*COLLAPSE+j].leftright.right)/(1<<7);
	    }
	  sum /= COLLAPSE*2;
	  assert(i + pos/COLLAPSE < audiosize/COLLAPSE);
	  data[i+pos/COLLAPSE]=sum;;
	}
      pos+=count;
    }
  
  fclose(raw);
  delete(progress);
  
  // normalize the audio
  unsigned4 maximum = 0;
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    if (data[i]>maximum)
      maximum=data[i];
  if (maximum==0)
    {
      audiosize=0;
      return;
    }
  for(int i = 0 ; i < audiosize/COLLAPSE ; i++)
    data[i]=(signed8)data[i]*(signed8)255/maximum;
}

void PatternAnalyzerLogic::showPattern()
{ 
  readFile(false);
  if (!audiosize)
    {
      QMessageBox::warning(this,"Fragment too small","There is simply no raw data on disk,\nHence, I can't display the beat graph");
      return;
    }
  if (!period)
    {
      QMessageBox::warning(this,"No period estimate","No period estimate, hence cannot show the beat graph.\n"
			   "Please go to the bpm counter and measure the tempo first");
      return;
    }
  int beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period  / COLLAPSE ;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  int window_xsize = collapsed_size / collapsed_period - 1 ;
  int window_ysize = pattern->contentsRect().height();
  int dc = color->value();
  assert(window_ysize>0);
  QPixmap *pm = new QPixmap(window_xsize,window_ysize);
  QPainter p;
  p.begin(pm);
  float yscale = collapsed_period - 1 ;
  yscale /= window_ysize;
  array(project, window_ysize, signed4);
  for(int row =  0 ; row < window_ysize ; row ++)
    project[row]=0;
  for(int column = 0 ; column < window_xsize ; column++)
    {
      unsigned4 idx = column * collapsed_period;
      if (::x/(normalperiod*beats_per_column/4) == column)
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,0,0));
	    p.drawPoint(column,row);
	  }
      else
	for(int row = 0 ; row < window_ysize ; row++)
	  {
	    unsigned4 idx2 = (int)((float)row*yscale);
	    assert(idx+idx2<collapsed_size);
	    int val = data[idx+idx2];
	    project[row]+=val;
	    val+=dc;
	    val%=256;
	    p.setPen(QColor(val,val,val));
	    p.drawPoint(column,row);
	  }
    }
  p.setPen(QColor(0,(128+dc)%256,0));
  for(int row = 0 ; row < window_ysize ; row+=window_ysize/8)
    p.drawLine(0,row,window_xsize-1,row);
  p.end(); 
  pattern->setPixmap(*pm);
  signed4 maximum=0;
  for(int row = 0 ; row < window_ysize ; row ++)
    if (project[row]>maximum)
      maximum=project[row];
  for(int row = 0 ; row < window_ysize ; row ++)
    project[row]=((signed8)project[row])*(signed8)255/(signed8)maximum;
  QPixmap *sm = new QPixmap(1,window_ysize);
  QPainter l;
  l.begin(sm);
  for(int row = 0 ; row < window_ysize ; row++)
    {
      int val;
      val = project[row];
      l.setPen(QColor(val,val,val));
      l.drawPoint(0,row);
    }
  l.end();
  projection->setPixmap(*sm);
}

/*void PatternAnalyzerLogic::run()
{ 
  unsigned int row, beats_per_column = beats->value() ;
  unsigned4 collapsed_period = period;
  collapsed_period *= beats_per_column;
  collapsed_period /= 4 * COLLAPSE;
  unsigned4 collapsed_size = audiosize / COLLAPSE ;
  array(pattern,collapsed_period, double);
  for(row =  0 ; row < collapsed_period ; row ++)
    pattern[row]=0;
  for(unsigned4 pos = 0 ; pos < collapsed_size ; pos ++)
    pattern[pos%collapsed_period]+=data[pos];
  double maximum = 0;
  for(row = 0 ; row < collapsed_period ; row ++)
    if (fabs(pattern[row])>maximum) maximum = fabs(pattern[row]);
  for(row = 0 ; row < collapsed_period ; row ++)
    pattern[row]=fabs(pattern[row]*255.0/maximum);
  for(row = 0 ; row < collapsed_period ; row ++)
    if (pattern[row]==255.0)
      break;
  int phase = row;
*/
  // hier zitten we met het probleem van de boel op te slaan op disk...
  // we willen dat zo klein mogelijk...
  // en toch zoveel mogelijk nuttige informatie behouden...
  // die textfiles 
  // in karakters kan ik het misschien der nog bijproppen
/*  array(pattern_for_index,collapsed_period , unsigned char);
  for(row = 0 ; row < collapsed_period ; row ++)
    pattern_for_index[row]=(unsigned char)pattern[(phase+row)%collapsed_period];
  index_pattern = pattern_for_index;
  index_pattern_size = collapsed_period;
  index_changed = 1;
  index_write();
}

void PatternAnalyzerLogic::dumpPatternToIdx()
{
  run();
}

*/

void PatternAnalyzerLogic::slantChanged()
{
  period=normalperiod+periodDelta->value()+periodDelta10->value();
  if (period<=0) period=1;
  showPattern();
}

void PatternAnalyzerLogic::balanceChanged()
{
  showPattern();
}

void PatternAnalyzerLogic::setTempo()
{
  normalperiod = normalperiod+periodDelta->value()+periodDelta10->value();
  if (normalperiod<=0) normalperiod = 1;
  ::y = ::x * currentperiod / normalperiod;
  periodDelta->setValue(0);
  periodDelta10->setValue(0);
  playing->set_period(normalperiod/4);
}


//---------------------------------------------------------------------
//
//  creation of an ultra accurate filter
//
//---------------------------------------------------------------------
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
  free(coef);
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
  free(filter);
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
  lo = atof(locut->currentText());
  hi = atof(hicut->currentText());
  if (lo<0) lo = 0;
  if (hi>WAVRATE/2) hi = WAVRATE / 2;
  if (lo>=hi) lo = 0; change = true; 
  if (hi<=lo) hi = WAVRATE / 2;
  filtersize = atoi(order->text());
  // build new filter and show image...
  createFilter();
  showFilter();
  // release data so it will be read later on..
  free(data);
  data=NULL;
  audiosize=0;
}


//---------------------------------------------------------------------
//
//  rythm analysis
//
//---------------------------------------------------------------------
// The rythm analysis contains 6 parts
// red & blue : fourier analysis of the signal
// green & yellow : autocorrelation sequence

void PatternAnalyzerLogic::calculateRythmPattern()
{
  int **** data; // make sure shadowing happens
  int **** audiosize;
  int shrinker = COLLAPSE;
  signed8 samples;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  FILE * raw=openRawFile(playing,arg_rawpath);
  samples=fsize(raw)/4;
  if (samples == 0) return;
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  signed8 length = 2;
  for(signed8 shifter = samples ; shifter > 1 ; shifter/=2, length*=2);
  assert(length>=samples);
  length/=shrinker;
  double * audio = allocate(length,double);
  printf("Pattern-Analyzer: allocated data-store with size %d for %d samples\n",(int)length,(int)samples);

  QProgressDialog * progress = NULL;
  progress = new QProgressDialog();
  progress->setTotalSteps(samples/bufsize);
  progress->show();
  
  while(pos<samples)
    {
      signed8 toread = samples - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      if (progress) progress->setProgress(pos/bufsize);
      app->processEvents();
      
      for(int i = 0 ; i < count; i++)
	audio[(pos+i)/shrinker]=(buffer[i].leftright.left)+(buffer[i].leftright.right);
      pos+=count;
    }
  printf("Pattern-Analyzer: Reading %ld samples\n",samples);
  samples/=shrinker;
  
  fclose(raw);
  delete(progress);
  
  // centralize the audio
  double total = 0;
  for(int i = 0 ; i < samples ; i++)
    total+=audio[i];
  total/=samples;
  if (total!=0)
    for(int i = 0 ; i < samples ; i++)
      audio[i]-=total;
  printf("Pattern-Analyzer: Removed DC offset %g\n",total);

  // clear the remainder of the array
  for(int i = samples ; i < length ; i ++)
    audio[i]=0;
  
  // normalize the audio
  double maximum = 0;
  for(int i = 0 ; i < samples ; i++)
    {
      double sample = fabs(audio[i]);
      if (sample>maximum)
	maximum=sample;
    }
  if (maximum==0) return;
  for(int i = 0 ; i < samples ; i++)
    audio[i]/=maximum;
  printf("Pattern-Analyzer: Normalized audio %g\n",maximum);

  // create the enveloppe
  array(env,length,double);
  for(int i = 0 ; i < length ; i ++)
    env[i]=fabs(audio[i]);
  printf("Pattern-Analyzer: Created audio enveloppe\n");
  
  // do the fourier transformation of the enveloppe
  double * env_fr = allocate(length,double);
  double * env_fi = allocate(length,double);
  fft_double(length,false,env,NULL,env_fr, env_fi);
  for(int i = 0 ; i < length ; i ++)
    {
      double re = env_fr[i];
      double im = env_fi[i];
      env_fr[i]=sqrt(re*re+im*im);
      env_fi[i]=atan2(im,re);
    }
  printf("Pattern-Analyzer: Obtained enveloppe spectrum\n");
  
  // obtain beat information
  array(notes_energy,64,double);
  array(notes_phase,64,double);
  // the normaperiod is the length of onbe measure, 
  // which is assumed to be 4 beats
  double beat_freq_in_bpm = mperiod2bpm(normalperiod);
  double measures_per_sec = beat_freq_in_bpm/(4.0*60.0);
  // notes[i] contains the strength of 1/i notes, or the frequency tempo/i
  // measures[i] contains the strength of 32/i notes
  for(int x = 0 ; x < 32 ; x ++)
    {
      // calculate the right part
      double startfreq = measures_per_sec*(((double)(x+1))-0.1);
      double stopfreq = measures_per_sec*(((double)(x+1))+0.1);
      double start_index = startfreq * length / (double)WAVRATE;
      double stop_index = stopfreq * length / (double)WAVRATE;
      double energy = 0;
      double phase = 0;
      for(int i = start_index ; i < stop_index ; i ++)
	{
	  energy+=env_fr[i];
	  phase+=env_fi[i];
	}
      energy/=(stop_index-start_index);
      phase/=(stop_index-start_index);
      while (phase<0) phase+=2*M_PI;
      notes_energy[x+32]=log(energy);
      notes_phase[x+32]=phase;
      
      // calculate the left part
      startfreq /= (32.0-x);
      stopfreq /= (32.0-x);
      start_index = startfreq * length / (double)WAVRATE;
      stop_index = stopfreq * length / (double)WAVRATE;
      energy = phase = 0;
      for(int i = start_index ; i < stop_index ; i ++)
	{
	  energy+=env_fr[i];
	  phase+=env_fi[i];
	}
      energy/=(stop_index-start_index);
      phase/=(stop_index-start_index);
      while (phase<0) phase+=2*M_PI;
      notes_energy[x]=log(energy);
      notes_phase[x]=phase;
    }
  
  // do the autocorrelation
  double * tmp_fr = env_fr; env_fr = NULL;  // reuse array
  double * tmp_fi = env_fi; env_fi = NULL;  // reuse array
  fft_double(length, false, audio, NULL, tmp_fr, tmp_fi);
  printf("Pattern-Analyzer: Autocorrelation forward transform\n");
  for(int i = 0 ; i < length ; i ++)
    tmp_fr[i]=tmp_fr[i]*tmp_fr[i]+tmp_fi[i]*tmp_fi[i];
  printf("Pattern-Analyzer: Noramlised for the next step\n");
  double * cor_r = audio; audio = NULL; // reuse array
  double * cor_i = env; env = NULL; // reuse array
  fft_double(length,true,tmp_fr, NULL, cor_r, cor_i);
  free(tmp_fr);
  free(tmp_fi);  
  free(cor_i);
  // env   audio
  // env   audio env_fr env_fi
  // env   audio tmp_fr tmp_fi
  // cor_i cor_r tmp_fr tmp_fi
  printf("Pattern-Analyzer: Calculated autocorrelation\n");
  
  // obtain beat information based on autocorrelation
  array(notes_cor,64,double);
  // the length of a beat can be calculated based on 
  double note_period = normalperiod;  // normalperiod is expressed in measures
  for(int x = 0 ; x < 32 ; x ++)
    {
      // the right part
      double start_index = note_period/(((double)x)+1.1);
      double stop_index = note_period/(((double)x)+0.9);
      double energy = 0;
      for(int i = start_index ; i < stop_index ; i ++)
	if (cor_r[i] > energy) energy=cor_r[i];
      notes_cor[x+32]=energy;
      
      // the left part
      start_index *= (32-x);
      stop_index *= (32-x);
      energy = 0;
      for(int i = start_index ; i < stop_index ; i ++)
	if (cor_r[i] > energy) energy=cor_r[i];
      notes_cor[x]=energy;
    }

  // normalize differente ranges
  double minimum = notes_energy[32];
  for(int i = 0 ; i < 64 ; i ++)
    if (notes_energy[i]<minimum)
      minimum=notes_energy[i];
  for(int i = 0 ; i < 64 ; i ++)
    notes_energy[i]-=minimum;
  maximum = 0;
  for(int i = 0 ; i < 64 ; i ++)
    if (notes_energy[i]>maximum)
      maximum=notes_energy[i];
  if (maximum>0)
    for(int i = 0 ; i < 64 ; i ++)
      notes_energy[i]/=maximum;

  minimum = notes_cor[32];
  for(int i = 0 ; i < 64 ; i ++)
    if (notes_cor[i]<minimum)
      minimum=notes_cor[i];
  for(int i = 0 ; i < 64 ; i ++)
    notes_cor[i]-=minimum;
  maximum = 0 ;
  for(int i = 0 ; i < 64 ; i ++)
    if (notes_cor[i]>maximum)
      maximum=notes_cor[i];
  if (maximum>0)
    for(int i = 0 ; i < 64 ; i ++)
      notes_cor[i]/=maximum;
  
  for(int i = 32 ; i < 64 ; i ++)
    {
      double phase = notes_phase[i]-notes_phase[32];
      while (phase<0) phase += 2*M_PI;
      while (phase>M_PI) phase -= 2*M_PI;
      notes_phase[i]=phase;
    }
  notes_phase[32]=0;
  
  // draw everything
  int demo_size_x = 400;
  int demo_size_y = 64;
  QPixmap *pm1 = new QPixmap(40,100);
  QPixmap *pm2 = new QPixmap(demo_size_x,demo_size_y);
  QPainter p, d;
  p.begin(pm1);
  d.begin(pm2);
  QRect r(0,0,40,100), s(0,0,demo_size_x,demo_size_y);
  p.fillRect(r,Qt::white);
  d.fillRect(s,Qt::white);
  for(int i = 63 ; i >=24 ; i --)
    {
      int x = (i - 24);
      double mean_energy=(notes_energy[i]+notes_cor[i])/2;
      p.setPen(Qt::gray);
      p.drawLine(x,99,x,(int)(99.0-(mean_energy*99.0)));
      p.setPen(Qt::green);
      p.drawPoint(x,(int)(99.0-(notes_energy[i]*99.0)));
      p.setPen(Qt::yellow);
      p.drawPoint(x,(int)(99.0-(notes_cor[i]*99.0)));
      if (i==32)
	{
	  p.setPen(Qt::black);
	  p.drawPoint(x,0);
	  p.drawPoint(x,99);
	}
      if (i>31)
	{
	  p.setPen(Qt::blue);
	  p.drawPoint(x,(int)(50.0-(notes_phase[i]*49.0/M_PI)));
	}
    }
  p.end();
  rythm_pattern->setPixmap(*pm1);
  
  d.setPen(Qt::gray);
  for(int i = 0 ; i < 8 ; i ++)
    {
      int x = i * demo_size_x / 8;
      d.drawLine(x,0,x,63);
    }
  
  d.setPen(Qt::black);
  for(double x = 0 ; x < demo_size_x ; x +=1.0)
    {
      double amplitude = 0;
      double rx = x*2.0*M_PI/((double)demo_size_x);
      for(int i = 0 ; i < 32 ; i ++)
	{
	  double energy = (notes_energy[i+32]+notes_cor[i+32])/2;
	  double sine = cos((rx+notes_phase[i+32])*(i+1));
	  if (sine > 0.98)
	    amplitude += sine*energy/(double)(i+5); // +5 flattens the exponential curve by goign more towards its tail
	}
      int y=demo_size_y-1-demo_size_y*amplitude*3;
      d.drawPoint((int)x,y);
    }
  d.end();
  
  pattern_demo->setPixmap(*pm2);
  
  free(notes_phase);
  free(notes_cor);
  free(notes_energy);
}
