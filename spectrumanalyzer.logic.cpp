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
#include "spectrumanalyzer.logic.h"
#include "sys/times.h"
#include "fourier.h"
#include "kbpm-play.h"
#include "version.h"
#include "scripts.h"

double barkbounds[barksize+1] =
  {
    0,100,200,300,
    400,510,630,770,
    920,1080,1270,1480,
    1720,2000,2380,2700,
    3150,3700,4400,5300,
    6400,7700,9500,12000,
    15500
  };

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
  // we get all sliders and stuff them into the barkmeters
  meters[0]=bark0;
  meters[1]=bark1;
  meters[2]=bark2;
  meters[3]=bark3;
  meters[4]=bark4;
  meters[5]=bark5;
  meters[6]=bark6;
  meters[7]=bark7;
  meters[8]=bark8;
  meters[9]=bark9;
  meters[10]=bark10;
  meters[11]=bark11;
  meters[12]=bark12;
  meters[13]=bark13;
  meters[14]=bark14;
  meters[15]=bark15;
  meters[16]=bark16;
  meters[17]=bark17;
  meters[18]=bark18;
  meters[19]=bark19;
  meters[20]=bark20;
  meters[21]=bark21;
  meters[22]=bark22;
  meters[23]=bark23;
  // and of course we all reset them to zero...
  for (int i = 0 ; i < barksize; i++)
    meters[i]->setValue(0);
}

void SpectrumDialogLogic::fetchSpectrum_normal()
{
  long int slidesize = WAVRATE*10;
  long int slide;
  long int blocksize = 65536;
  double *fftdata;
  double *fftfreq;
  double *fftfreqi;
  double *fftwindowfreq;
  signed short *data;
  double *barkscale;
  int bark;
  barkscale = allocate(barksize,double);
  fftdata = allocate(blocksize+slidesize,double);
  fftwindowfreq = allocate(blocksize,double);
  fftfreq = allocate(blocksize/2,double);
  fftfreqi = allocate(blocksize,double);
  // hieronder x 2 omdat we zowel links als rechts binnen krijgen
  data=allocate((blocksize+slidesize)*2 ,signed short);
  FILE * raw = openRawFile(playing, arg_rawpath);
  // reset the fftfreq
  long pos;
  for(pos=0;pos<blocksize/2;pos++)
    fftwindowfreq[pos]=0.0;
  // position file
  long startpos = cue ? cue * 4 : fsize(raw)/2;
  if (startpos+(blocksize+slidesize)*2<=fsize(raw))
    startpos = fsize(raw)/2;
  if (startpos+(blocksize+slidesize)*2<=fsize(raw))
    {
      // printf("Fetching spectrum at position = %d\n",(int)cue);
      fseek(raw,startpos,SEEK_SET);
      // read in memory
      pos=0;
      while(pos<blocksize+slidesize)
	pos += readsamples((unsigned4*)(data+pos),(blocksize+slidesize-pos)/2,raw)*2;
      // shrink down
      for(pos=0;pos<blocksize+slidesize;pos++)
	fftdata[pos]=(double)data[pos*2];
      // cummulate different windows
      for(slide=0;slide<slidesize;slide+=slidesize/100)
	{
	  Progress->setProgress(slide,slidesize);
	  app->processEvents();
	  // do an fft on that position and normalize the result
	  fft_double(blocksize,0,fftdata+slide,NULL,fftwindowfreq,fftfreqi);
	  for(pos=0;pos<blocksize/2;pos++)
	    fftfreq[pos]+=fftwindowfreq[pos];
	}
      Progress->setProgress(slidesize,slidesize);
      app->processEvents();
      fclose(raw);
      // normalize the result
      double max = 0;
      for(pos=0;pos<blocksize/2;pos++)
	// fftfreq[pos]=fabs(fftfreq[pos]*(double)pos/((double)blocksize*10.0));
	fftfreq[pos]=fabs(fftfreq[pos]);
      for(pos=0;pos<blocksize/2;pos++)
	if (fftfreq[pos]>max) 
	  max=fftfreq[pos];
      for(pos=0;pos<blocksize/2;pos++)
	fftfreq[pos]=fftfreq[pos]*100.0/max;
      
      // bring all frequency relatively to the bark-scales
      for(bark=0;bark<barksize;bark++)
	barkscale[bark]=0;
      for(pos=0;pos<blocksize/2;pos++)
	{
	  double freq = Index_to_frequency(blocksize,pos)*(double)WAVRATE;
	  for(bark=0;bark<barksize;bark++)
	    if (freq>barkbounds[bark] &&
		freq<barkbounds[bark+1])
	      {
		double length = barkbounds[bark+1]-barkbounds[bark];
		double barkcentre = barkbounds[bark]+length/2.0;
		double dist = fabs(freq-barkcentre)*2.0/length;
		double scale = 1.0 - dist;
		assert(scale>=0.0);
		barkscale[bark]+=fftfreq[pos]*scale;
		assert(barkscale[bark]>=0.0);
	      }
	}
      // rescale bark information..
      // the first entry in the scale is always halved because this is the bass level
      barkscale[0]/=2.0;
      max=0;
      for(bark=0;bark<barksize;bark++)
	if (barkscale[bark]>max)
	  max=barkscale[bark];
      for(bark=0;bark<barksize;bark++)
	{
	  barkscale[bark]=barkscale[bark]*99.0/max;
	  assert(barkscale[bark]>=0.0);
	}
      // update the meters...
      for(pos=0;pos<barksize;pos++)
	meters[pos]->setValue((int)barkscale[pos]);
      // write out values...
      char spectrum[barksize+1];
      for(pos=0;pos<barksize;pos++)
	spectrum[pos]='a'+(char)(barkscale[pos]/4);
      spectrum[barksize]=0;
      playing->set_spectrum(strdup(spectrum));
    }
  else
    {
      printf("Could not measure spectrum because there is not enough data at position (bytes) %d\n",(int)startpos);
    }
  // free everything involved
  deallocate(fftwindowfreq);
  deallocate(fftfreq);
  deallocate(fftfreqi);
  deallocate(fftdata);
  deallocate(data);
}

