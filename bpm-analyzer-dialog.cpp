/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__bpm_analyzer_dialog_cpp__
#define __loaded__bpm_analyzer_dialog_cpp__
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
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
#include <string.h>
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
#include <sys/mman.h>
#include <syscall.h>
#include <math.h>
#include "bpm-analyzer-dialog.h"
#include "sys/times.h"
#include "version.h"
#include "player-core.h"
#include "scripts.h"
#include "memory.h"
#include "clock-drivers.h"
#include "bpm-analyzer-wec.h"
#include "bpm-analyzer-2001.h"
#include "bpm-analyzer-2010.h"
#include "bpm-analyzer-autocor.h"
#include "bpm-analyzer-env.h"
#include "info.h"

#define IMAGE_XS 640
#define IMAGE_YS 320

BpmAnalyzerDialog::BpmAnalyzerDialog(QWidget*parent) : QWidget(parent)
{
  setupUi(this);
  audio=0;
  tapcount=0;
  startbpm = 120;
  stopbpm = 160;
  connect(&progress,SIGNAL(newProgress(int)),this,SLOT(updateProgress(int)));
  connect(&progress,SIGNAL(newStatusText(QString)),this,SLOT(status(QString)));
  connect(&progress,SIGNAL(finishedAnalyzing()),this,SLOT(finished()));
  // clear the image
  QPixmap *pm = new QPixmap(10,10);
  QPainter p;
  p.begin(pm);
  QRect r(QRect(0,0,pm->width(),pm->height()));
  p.fillRect(r,Qt::white);
  p.end();
  BpmPix->setPixmap(*pm);
}

void BpmAnalyzerDialog::setBpmBounds(signed4 start, signed4 stop)
{
  FromBpmEdit->setText(QString::number(startbpm=start));
  ToBpmEdit->setText(QString::number(stopbpm=stop));
}

void BpmAnalyzerDialog::updateProgress(signed4 reading_progress)
{
  ReadingBar->setValue(reading_progress);
}

void BpmAnalyzerDialog::status(QString text)
{
  StatusLabel->setText(text);
}

void BpmAnalyzerDialog::retrieve_image()
{
  QPixmap image(IMAGE_XS,IMAGE_YS);
  progress.paint(&image);
  BpmPix->setPixmap(image);
}

void BpmAnalyzerDialog::finished()
{
  assert(bpmanalimp);
  tempo_type result(bpmanalimp->tempo);
  set_measured_period_diskrate(tempo_to_period(result).period*4,true);
  retrieve_image();
  progress.clear_plot_data();
  bpmanalimp.reset();
  munmap(audio,map_length);
  StartStop->setEnabled(true);
}

void BpmAnalyzerDialog::set_measured_period_diskrate(signed4 p,bool update_on_disk)
{
  set_normalperiod_metarate(diskrate_to_metarate(p),update_on_disk);
}

void BpmAnalyzerDialog::startStopButton()
{
  if (!bpmanalimp)
    start();
}

void BpmAnalyzerDialog::start()
{
  assert(bpmanalimp==NULL);
  startbpm=FromBpmEdit->text().toInt();
  stopbpm=ToBpmEdit->text().toInt();
  printf("Starting analyzer\n");
  FILE * file = openCoreRawFile();
  assert(file);
  int fd = fileno(file);
  map_length = fsize(file);
  audio = (stereo_sample2*)mmap(NULL,map_length,PROT_READ,MAP_SHARED,fd,0);
  assert(audio!=MAP_FAILED);
  assert(audio);  
  if (enveloppeSpectrum->isChecked())
    {
      assert(diskrate==44100);
      bpmanalimp.reset(new BpmAnalyzerEnv(audio,map_length/4,startbpm,stopbpm));
    }
  else if (fullAutoCorrelation->isChecked())
    {
      assert(diskrate==44100);
      bpmanalimp.reset(new BpmAnalyzerAutocor(audio,map_length/4,startbpm,stopbpm));
    }
  else if (weightedEnvCor->isChecked()) 
    bpmanalimp.reset(new BpmAnalyzerWec(audio,map_length/4,diskrate,startbpm,stopbpm));
  else if (experimentalAlg->isChecked())
    bpmanalimp.reset(new BpmAnalyzer2010(audio,map_length/4,diskrate,startbpm,stopbpm));
  else if (resamplingScan->isChecked())
    {
      assert(diskrate==44100);
      bpmanalimp.reset(new BpmAnalyzer2001(audio,map_length/4,startbpm,stopbpm));
    }
  assert(bpmanalimp);
  bpmanalimp->attach_progress(&progress);
  bpmanalimp->attach_plot(&progress);
  StartStop->setEnabled(false);
  worker.run(bpmanalimp);
}

void BpmAnalyzerDialog::terminate()
{
  worker.terminate();
}

void BpmAnalyzerDialog::reset()
{
  tapcount = 0;
  TapLcd->display(tapcount);
}

void BpmAnalyzerDialog::tap()
{
  if (++tapcount==1) 
    starttime=times(NULL);
  else
    {
      /**
       * we have to decrease tapcount with one because at tick 2 the 
       * time passed counts for only one beat.
       */
      signed8 p = (((signed8)times(NULL)-(signed8)starttime)/(tapcount-1));
      p *= normalperiod_metarate;
      p /= currentperiod_metarate;
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
      signed8 oldcurrentperiod_metarate=currentperiod_metarate;
      signed8 oldnormalperiod_metarate=normalperiod_metarate;
      set_measured_period_diskrate(period_to_quad(p),false);
      ::metronome->changetempo(normalperiod_metarate*oldcurrentperiod_metarate/oldnormalperiod_metarate);
      
      tempo_type tempo = playing->get_tempo();
      setBpmBounds(tempo.lower(10),tempo.higher(10));
    }
  TapLcd->display(tapcount);
}
#endif // __loaded__bpm_analyzer_dialog_cpp__
