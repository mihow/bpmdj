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

extern "C"
{
#include "cbpm-index.h"
#include "version.h"
#include "player-core.h"
#include "scripts.h"
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
  index_bpmcount_from = 0;
  index_bpmcount_to = 100;
  bufsiz = 32 * 1024;
  // internal communicative variables
  reading_progress = 0;
  processing_progress = 0;
  // read the index file
  char d[1024];
  sprintf(d,"BpmDj v%s",VERSION);
  index_version=strdup(d);
  // clear the image
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
  BpmPix->setPixmap(*pm);
  // set index filen filename
  IdxEdit->setText(index_readfrom);
  // set tag line
  if (index_tags)
    TagEdit->setText(index_tags);
  else
    TagEdit->setText("");   
  // set song 
  if (index_file)
    SongLabel->setText(index_file);
}

unsigned long BpmAnalyzerDialog::phasefit(unsigned long i)
{
  unsigned long c;
  unsigned long mismatch=0;
  assert(i<audiosize);
  for(c=i;c<audiosize;c++)
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
   index_bpmcount_from=startper;
   index_bpmcount_to=stopper;
   sprintf(tmp,"%d",index_bpmcount_from);
   FromPercentEdit->setText(tmp);
   sprintf(tmp2,"%d",index_bpmcount_to);
   ToPercentEdit->setText(tmp2);
}

void BpmAnalyzerDialog::timerTick()
{
  ReadingBar->setProgress(reading_progress);
  CountingBar->setProgress(processing_progress);
}

void BpmAnalyzerDialog::readAudio()
{
  FILE * raw = openRawFile(arg_rawpath);
  
  // read complete file shrunken down into memory
  audiosize=fsize(raw);
  long startpercent=(long)((long long)audiosize*(long long)index_bpmcount_from/(long long)100);
  startpercent-=startpercent%4;
  long stoppercent=(long)((long long)audiosize*(long long)index_bpmcount_to/(long long)100);
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
	  left=abs(buffer[i]);
	  right=abs(buffer[i+1]);
	  mean=(left+right)/2;
	  redux=abs(mean)/128;
	  if (pos+i/(2*(WAVRATE/audiorate))>=audiosize) break;
	  assert(pos+i/(2*(WAVRATE/audiorate))<audiosize);
	  audio[pos+i/(2*(WAVRATE/audiorate))]=(unsigned char)redux;
	}
      pos+=count/(WAVRATE/audiorate);
    }
  fclose(raw);
  if (stop_signal)
    {
      free(audio);
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
    if (changed |= (unsigned)index_bpmcount_from != val) 
      index_bpmcount_from=val;
    val = atoi(ToPercentEdit->text());
    if (changed |= (unsigned)index_bpmcount_to != val) 
      index_bpmcount_to=val;
    val = atoi(FromBpmEdit->text());
    if (startbpm != val)
      startbpm=val;
    val = atoi(ToBpmEdit->text());
    if (stopbpm != val)
      stopbpm=val;
    if (changed && audio)
      {
	free(audio);
	audio=NULL;
	audiosize=0;
      }
  }

void BpmAnalyzerDialog::run()
{
  char d[500];
  unsigned long i;
  
  // check wether ranges are changed
  rangeCheck();
  
  if (stop_signal)
    return;
  if (!audio)
    readAudio();
  if (stop_signal)
    return;
  
  // now process the sucker 
  StatusLabel->setText("Finding best autocorrelation");
  sprintf(d," ");
  
  stopshift=audiorate*60*4/startbpm;
  startshift=audiorate*60*4/stopbpm;
  {
    unsigned long foutat[stopshift-startshift];
    unsigned long fout, minimumfout=0, maximumfout=(unsigned)-1,minimumfoutat=0,left,right;
    memset(&foutat,0,sizeof(foutat));
    
    // how many times wil we execute phasefit ???
    // stopshift-startshift/50 + 200
    int O = ((stopshift-startshift) / 50)+200;
    int o = 0;
    for(i=startshift;i<stopshift && !stop_signal;i+=50)
      {
	fout=phasefit(i);
	o++;
	processing_progress = o*100/O;
	foutat[i-startshift]=fout;
	if (minimumfout==0) maximumfout=minimumfout=fout;
	if (fout<minimumfout) 
	  {
	    minimumfout=fout;
	    minimumfoutat=i;
	  }
	if (fout>maximumfout) maximumfout=fout;
      }
    if (stop_signal)
      {
	StatusLabel->setText("Canceled while autocorrelating");
	return;
      }
    if (minimumfoutat>=100) left=minimumfoutat-100;
    else left=0;
    if (left<startshift) left=startshift;
    right=left+200;
    if (right>stopshift) right=stopshift;
    for(i=left;i<right && !stop_signal;i++)
      {
	fout=phasefit(i);
	o++;
	processing_progress = o*100/O;
	foutat[i-startshift]=fout;
	//	     printf("# %d: %ld (%g BPM)\n",i,fout,
	//		    4.0*(double)audiorate*60.0/(double)i);
	if (minimumfout==0) maximumfout=minimumfout=fout;
	if (fout<minimumfout) 
	  {
	    minimumfout=fout;
	    minimumfoutat=i;
	  }
	if (fout>maximumfout) maximumfout=fout;
      }
    if (stop_signal)
      {
	StatusLabel->setText("Canceled while autocorrelating");
	return;
      }
    processing_progress = 100;
    StatusLabel->setText("Ready");
    index_period=minimumfoutat;
    index_changed=1;
    double tempo = 4.0*(double)audiorate*60.0/(double)minimumfoutat;
    sprintf(d,"%g",tempo);
    index_tempo=strdup(d);
    
    normalperiod=index_period*4;
    currentperiod=normalperiod;
    if (WAVRATE==22050)
      {
	currentperiod/=2;
	normalperiod/=2;
      }
    
    // if possible we should draw this !
    QPixmap *pm = new QPixmap(640,640);
    QPainter p;
    p.begin(pm);
    QRect r(QRect(0,0,pm->width(),pm->height()));
    p.fillRect(r,Qt::white);
    // now we should draw the vertical lines at 1/4; 2/4; 3/4
    int X,Y;
    p.setPen(Qt::green);
    p.drawLine(320,0,320,639);
    p.drawLine(160,0,160,639);
    p.drawLine(480,0,480,639);
    p.setPen(Qt::red);
    X = (minimumfoutat - startshift) * r.width() / (stopshift - startshift);
    X = r.width() - X;
    p.drawLine(X,0,X,639);
    
    // draw points
    p.setPen(Qt::black);
    for(i=startshift;i<stopshift;i++)
      {
	fout=foutat[i-startshift];
	if (fout)
	  {
	    fout-=minimumfout;
	    X = (i - startshift) * r.width() / (stopshift - startshift);
	    X = r.width() - X;
	    Y = (int)((long long)fout * (long long)r.height() / (long long) (maximumfout-minimumfout));
	    Y = r.height() - Y;
	    p.drawPoint(X,Y);
	    p.drawPoint(X+1,Y);
	    p.drawPoint(X,Y+1);
	    p.drawPoint(X+1,Y+1);
	  }
      }
    // set labels...
    sprintf(d,"%2g",4.0*(double)audiorate*60.0/stopshift);
    X0->setText(d);
    sprintf(d,"%2g",4.0*(double)audiorate*60.0/(startshift+3*(double)(stopshift-startshift)/4.0));
    X1->setText(d);
    sprintf(d,"%2g",4.0*(double)audiorate*60.0/(startshift+(double)(stopshift-startshift)/2.0));
    X2->setText(d);
    sprintf(d,"%2g",4.0*(double)audiorate*60.0/(startshift+(double)(stopshift-startshift)/4.0));
    X3->setText(d);
    sprintf(d,"%2g",4.0*(double)audiorate*60.0/startshift);
    X4->setText(d);
    
    p.end();
    BpmPix->setPixmap(*pm);
  }
}

void BpmAnalyzerDialog::stoppedAnalyzing()
{
  Analyzer::stoppedAnalyzing();
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
  Analyzer::startAnalyzer();
}

void BpmAnalyzerDialog::stopAnalyzer()
{
  StatusLabel->setText("Canceling...");
  Analyzer::stopAnalyzer();
}

void BpmAnalyzerDialog::finish()
{
  Analyzer::finish();
  accept();
}

void BpmAnalyzerDialog::store()
{
  if (strcmp(index_readfrom,IdxEdit->text())!=0)
    {
      index_changed=1;
      index_readfrom=strdup(IdxEdit->text());
    }
  if (strcmp(index_tags,TagEdit->text())!=0)
    {
      index_tags=strdup(TagEdit->text());
      index_changed=1;
    }
  if (index_changed)
    index_write();
}

void BpmAnalyzerDialog::reset()
{
  tapcount = 0;
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::tap()
{
  char d[500];
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
      index_period=(((signed8)times(NULL)-(signed8)starttime)/(tapcount-1));
      index_period/=SkipBox->value();
      index_period*=11025*4;
      index_period/=CLOCK_FREQ;
      normalperiod=index_period*4;
      currentperiod=normalperiod;
      if (WAVRATE==22050)
	{
	  currentperiod/=2;
	  normalperiod/=2;
	}
      double tempo = 4.0*(double)11025*60.0/(double)index_period;
      sprintf(d,"%g",tempo);
      if (index_tempo)
	free(index_tempo);
      index_tempo=strdup(d);
      index_changed=1;
      setBpmBounds((long)(tempo-10.0),(long)(tempo+10.0));
    }
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::incBpm()
{
  index_period-=10;
  if(index_period<100) 
    index_period=100;
  normalperiod=index_period*4;
  currentperiod=normalperiod;
  if (WAVRATE==22050)
    {
      currentperiod/=2;
      normalperiod/=2;
    }
}

void BpmAnalyzerDialog::decBpm()
{
  index_period+=10;
  normalperiod=index_period*4;
  currentperiod=normalperiod;
  if (WAVRATE==22050)
    {
      currentperiod/=2;
      normalperiod/=2;
    }
}
