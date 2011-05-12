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

#include "pattern-analyzer.logic.h"

#ifdef RYTHM
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
#include "spectrumanalyzer.logic.h"
#include "sys/times.h"
#include "kbpm-play.h"
#include "fourier.h"
#include "version.h"
#include "scripts.h"

//---------------------------------------------------------------------
//
//  rythm analysis
//
//---------------------------------------------------------------------
// The rythm analysis contains 6 parts
// red & blue : fourier analysis of the signal
// green & yellow : autocorrelation sequence


void PatternAnalyzerLogic::calculateRythmPattern1()
{
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
      for(int i = (int) start_index ; i < stop_index ; i ++)
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
      for(int i = (int) start_index ; i < stop_index ; i ++)
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
  printf("Pattern-Analyzer: Normalised for the next step\n");
  double * cor_r = audio; audio = NULL; // reuse array
  double * cor_i = env; env = NULL; // reuse array
  fft_double(length,true,tmp_fr, NULL, cor_r, cor_i);
  deallocate(tmp_fr);
  deallocate(tmp_fi);
  deallocate(cor_i);
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
      for(int i = (int)start_index ; i < stop_index ; i ++)
	if (cor_r[i] > energy) energy=cor_r[i];
      notes_cor[x+32]=energy;
      
      // the left part
      start_index *= (32-x);
      stop_index *= (32-x);
      energy = 0;
      for(int i = (int)start_index ; i < stop_index ; i ++)
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
      int y=(int)(demo_size_y-1-demo_size_y*amplitude*3);
      d.drawPoint((int)x,y);
    }
  d.end();
  
  pattern_demo->setPixmap(*pm2);
  
  deallocate(notes_phase);
  deallocate(notes_cor);
  deallocate(notes_energy);
}

signed8 PatternAnalyzerLogic::calculateRythmPattern2(QPainter & d, int demo_size_x, int demo_size_y, signed8 start, signed8 samples)
  // samples is uitegedrukt in audiosamples (1 audiosample is typical 4 bytes)
  // the start is also expressed in audiosamples
  // the return is the new divisor relative to start
{
  int shrinker = COLLAPSE;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  FILE * raw=openRawFile(playing,arg_rawpath);
  signed8 file_size = fsize(raw)/4;
  if (samples<0)
    samples = file_size;
  if (start+samples>file_size)
    samples = file_size-start;
  if (samples == 0) return -1;
  fseek(raw, start * 4, SEEK_SET);
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  signed8 length = 2;
  for(signed8 shifter = samples ; shifter > 1 ; shifter/=2, length*=2);
  assert(length>=samples);
  length/=shrinker;
  printf("Pattern-Analyzer: Allocating data-store with size %d for %d samples\n",(int)length,(int)samples);
  double * audio = allocate(length,double);


  printf("Pattern-Analyzer: Reading %ld samples\n",samples);
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
  samples/=shrinker;

  if (samples<132300) 
    return -1;
  
  fclose(raw);
  delete(progress);
  
  // centralize the audio
  printf("Pattern-Analyzer: Removing DC offset\n");
  double total = 0;
  for(int i = 0 ; i < samples ; i++)
    total+=audio[i];
  total/=samples;
  if (total!=0)
    for(int i = 0 ; i < samples ; i++)
      audio[i]-=total;
  printf("Pattern-Analyzer: Removed DC offset %g\n",total);

  // clear the remainder of the array
  printf("Pattern-Analyzer: Clearing remaingder of array\n");
  for(int i = samples ; i < length ; i ++)
    audio[i]=0;
  
  // normalize the audio
  printf("Pattern-Analyzer: Normalizing audio\n");
  double maximum = 0;
  for(int i = 0 ; i < samples ; i++)
    {
      double sample = fabs(audio[i]);
      if (sample>maximum)
	maximum=sample;
    }
  if (maximum==0) return -1;
  for(int i = 0 ; i < samples ; i++)
    audio[i]/=maximum;

  // create the enveloppe
  printf("Pattern-Analyzer: Creating audio enveloppe\n");
  array(env,length,double);
  for(int i = 0 ; i < length ; i ++)
    env[i]=fabs(audio[i]);
  
  // do the fourier transformation of the enveloppe
  printf("Pattern-Analyzer: Obtaining enveloppe spectrum\n");
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
  
  // do the autocorrelation
  printf("Pattern-Analyzer: Autocorrelation forward transform\n");
  double * tmp_fr = env_fr; env_fr = NULL;  // reuse array
  double * tmp_fi = env_fi; env_fi = NULL;  // reuse array
  fft_double(length, false, audio, NULL, tmp_fr, tmp_fi);
  printf("Pattern-Analyzer: Obtaining complex norm\n");
  for(int i = 0 ; i < length ; i ++)
    tmp_fr[i]=tmp_fr[i]*tmp_fr[i]+tmp_fi[i]*tmp_fi[i];
  printf("Pattern-Analyzer: Autocorrelation backward transform\n");
  double * cor_r = audio; audio = NULL; // reuse array
  double * cor_i = env; env = NULL; // reuse array
  fft_double(length,true,tmp_fr, NULL, cor_r, cor_i);
  deallocate(tmp_fr);
  deallocate(tmp_fi);
  deallocate(cor_i);
  // env   audio
  // env   audio env_fr env_fi
  // env   audio tmp_fr tmp_fi
  // cor_i cor_r tmp_fr tmp_fi
  
  printf("Pattern-Analyzer: Removing sign from autocorrelation sequence\n");
  for(int i = 0 ; i < length ; i ++)
    cor_r[i]=fabs(cor_r[i]);
  
  
  // HACK !!! 
  length = 132300 ;
  // length/=256;  
  
  
  // to make sure that the head is not accounted too much we 
  // add some factor to it  the reasoning her goes as follows
  // white noise from the form 1/(1-a.e^(-jw)) has a C_hh(e^-jw) of
  // |1/(1-a.e^(-jw))|^2. The inverse fourier transform of this is a sequence
  // c_h = a^(2h). By subtracting this sequence from the actual autocorrelation
  // we remove the impact of the standard frequence
  // the only problem yet is to undesrtand the standard frequency distribution
  // the value for a for this curve is determined by the best fit we can obtain 
  printf("Pattern-Analyzer: Determining noise parameter\n");
  maximum = cor_r[0];
  double a = 0;
  for(int i = 1 ; i < length ; i++)
    a += pow(cor_r[i]/maximum,1/(2*(double)i));
  a/=length;
  printf("Pattern-Analyzer: Noise parameter is %g\n",a);
  
  printf("Pattern-analyzer: Adding shaping curve\n");
  for(int i = 0 ; i < length ; i ++)
    cor_r[i]-=maximum*pow(a,2*i);
  
  
  maximum=0;
  int at=0;
  printf("Pattern-anbalyzer: selecting peak\n");
  for(int i = 0 ; i < length ; i ++)
    if (cor_r[i]>maximum)
      {
	maximum=cor_r[i];
	at=i;
      }
  for(int i = 0 ; i < length ; i ++)
    cor_r[i]/=maximum;
  printf("Pattern-anbalyzer: selected peak is at %d\n",at);
  
  // draw everything
  int lasty=0;
  int lastx=0;
  for(int x = 0 ; x < demo_size_x ; x++)
    {
      int a = x*length/demo_size_x;
      int b = (x+1)*length/demo_size_x;
      if (b>=length) b=length-1;
      if (a>=length) a=length-1;
      double mean = 0;
      for(int i = a ; i < b ; i ++)
	if (cor_r[i]>mean)
	  mean=cor_r[i];
      //mean+=cor_r[i];
      //mean/=b-a+1;
      int y = (int)(((double)demo_size_y)*mean);
      d.drawLine(lastx,lasty,x,y);
      lastx=x;
      lasty=y;
    }
  deallocate(cor_r);

  // calculate maximum that will split the file
  // this strange writing is necesarry to make _use_ of rounding errors in integer divisions
  signed8 divl = ((samples/at)/2)*at;
  signed8 divr = divl+at;
  signed8 mid = samples/2;
  if (mid-divl<divr-mid) at = divl;
  else at = divr;
  printf("Pattern-analyzer: Length = %d, splitted at %d\n",(int)samples,(int)at);
  samples*=shrinker;
  at*=shrinker;
  return at;
}

void PatternAnalyzerLogic::calculateRythmPattern2(QPainter & d, int demo_size_x, int demo_size_y, signed8 start, signed8 samples, int depth)
{
  if (depth==5) return;
  // first draw the sucker
  QColor c;
  c.setHsv(depth*240/5,255,255);
  d.setPen(c);
  printf("Pattern-Analyzer: ==== Depth %d =================\n",depth);
  signed8 split = calculateRythmPattern2(d,demo_size_x,demo_size_y,start,samples);
  assert(split>0);
  // now do the division
  calculateRythmPattern2(d,demo_size_x,demo_size_y, start        , split, depth + 1);
  calculateRythmPattern2(d,demo_size_x,demo_size_y, start + split, split, depth + 1);
}

void PatternAnalyzerLogic::calculateRythmPattern2()
{
  int demo_size_x = 800;
  int demo_size_y = 400;
  QPixmap *pm2 = new QPixmap(demo_size_x,demo_size_y);
  QPainter d;
  d.begin(pm2);
  QRect r(0,0,demo_size_x,demo_size_y);
  d.fillRect(r,Qt::white);
  d.setPen(Qt::white);
  calculateRythmPattern2(d,demo_size_x,demo_size_y,0,-1,0);
  d.end();
  rythm_pattern->setPixmap(*pm2);  
}

void PatternAnalyzerLogic::calculateRythmPattern()
{
  if (autocorrelate->isChecked())
    calculateRythmPattern2();
  else
    calculateRythmPattern1();
}

#else

void PatternAnalyzerLogic::calculateRythmPattern()
{
}

#endif

/*
Mogelijkheden bij het bouwen van de tree

A -> AA
A -> AB
A -> BA
A -> BC
----

Comparison
A == B <==> left(A)==left(B) && right(A)==right(B)
if no left or right then A==B if autocorellation matches

Reductieregeles

A -> AA => A
A -> BC && D -> BC => A -> BC && A -> BC

Het beste is misschien bottom up te werken
De verschillende tekens bepalen in de muziek en aan de hand daarvan een boom opstellen
waarbij we gelijke stukken aanduiden. Daarbij zijn we dan vooral geinteresseerd in een 
zo compact mogelijke beschrijving van het geheel

meest voorkomende tweeklanken veranderen we naar een eigen letter. Dit proces herhalen we tot we geen 'meest voorkomende' meer hebben

na normalisatie natuurlijk van de gemiddelde afstand, rekeninghoudend met de standaarddeviatie...
ABAB -> 2xAB

Dan moeten we natuurlijk der nadien voor zorgen dan we 
Dit moet natuurlijk rekening houden met de timing van het geheel. 


WE hebben een reeks 'klein' tekens die we kunnen identificeren
Daarboven hebben we een reeks 'grotere' tekens die deze tekens spannen
Het voordeel aan het gebruik van correlatie sequenties is dat deze de phase straal negeren, wat ik vrij fantastisch vind.
Dus dit in combinatie met wavelets en we zijn vertrokken.


Nieuw algoritme...
1- We nemen de volledige autocorrelatie
2- We zoeken de eerste piek die na een zero crossing komt (of een ander systeem dat al even goed marcheert :)
3- We nemen het maximale veelvoud dat kleiner is dan de helft oftewel het minimale veelvoud groter dan de halve periode
4- We delen de boel in de opgegeven stukken en we herbeginnen het algoritme

Op deze manier kunnen we tempoveranderingen relatief gemakkelijk aan.
Het probleem is nu de vraag wanneer we stoppen met dat boeltje. 
Op deze manier kunnen we normaal een tree maken die steeds in 2 splitst. Waarbij we de afstand onder de bladen onderling kunnen
berekenen. Het compact opslaan van deze informatie kan gebeuren door veranderingen sets in te voeren. Dus de oneven bladeren bevatten 
de veranderingen ten opzichte van de vorige bladeren.

Hoe kunnen we dan 2 ritmes vergelijken ?
Er moet ergens een ritmische basis zijn die zo gevonden wordt. Doch waar vinden we deze ?
Hoe vergelijken we twee ritmes ? 
We tellen links bij rechts op of zoiets ?
  We moeten natuurlijk wel de autocorrelatie sequentie terug mappen op een relatief neutrale waarde, waarbij we de boel opdelen in 128 verschillende getallen. van 0 tot halverwege 

A van B willen aftrekken moeten we eerst de corellatie sequenties vergelijken van de ganse nummers. Deze zijn bruikbaar zoals ze zijn zonder rekening te houden met de schaal waarover de meting gebeurt is. 
  De volgende stap is de kinderen te vergelijken. Deze zouden ook gelijk moeten zijn. En dit doen we verder tot we op een deftig niveau uitkomen. Het probleem is de hoogte van de boom en de breedte. We moeten dus eigenlijk de grootste boom eerst opvouwen. 

Wanneer stoppen we het proces ? Op het ogenblik we geen piek meer vinden !

*/


