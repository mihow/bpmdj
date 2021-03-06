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
#ifndef __loaded__mixerdialog_logic_cpp__
#define __loaded__mixerdialog_logic_cpp__
using namespace std;
#include <cstdio>
#include <qlcdnumber.h>
#include <stdio.h>
#include <unistd.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qwidget.h>
#include <math.h>
#include <qslider.h>
#include <assert.h>
#include <qcheckbox.h>
#include <sys/times.h>
#include "version.h"
#include <stdlib.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qcombobox.h>
#include "bpmplay.h"
#include "mixerdialog.logic.h"
#include "memory.h"

MixerDialogLogic::MixerDialogLogic(QWidget*parent,const char*name, bool modal,
Qt::WindowFlags f) :
  MixerDialog(parent,name,modal,f)
{
  volumesChanged();
  stop=false;
}

void MixerDialogLogic::accept()
{
  stop = true;
  while(!stopped) ;
}

void MixerDialogLogic::reject()
{
  stop = true;
  while(!stopped) ;
}

void balance(int &a, int &b)
{
  if (a<b) balance(b,a);
  else if ( a + b != 0)
    {
      int c = a * a / ( a + b );
      int d = b * a / ( a + b );
      // printf("c: %d  d: %d\n",c,d);
      a = c;
      b = d;
      // since the mixing process will assume two channels we multiply the 
      // volume by 2
      a *= 2;
      b *= 2;
    }
}

void log_mapping(int a)
  {
    if (a==0) return;
    float8 b = a;
    float8 c = log((float8)dspmixmaxmult)/log(2);
    float8 d = b * c / (float8)dspmixmaxmult;
    float8 e = pow(2.0,d);
    // printf("%d -> %d\n",a,(int)e);
    a = (int)e;
  }

void MixerDialogLogic::volumesChanged()
{
  int _ch1main = - Ch1Main->value();
  int _ch2main = - Ch2Main->value();
  int _ch1monitor = - Ch1Monitor->value();
  int _ch2monitor = - Ch2Monitor->value();
  // if balanced then we 
  if (balanced->isOn())
    {
      balance(_ch1main,_ch2main);
      balance(_ch1monitor,_ch2monitor);
    }
  log_mapping(_ch1main);
  log_mapping(_ch2main);
  log_mapping(_ch1monitor);
  log_mapping(_ch2monitor);

  ch1main = _ch1main;
  ch2main = _ch2main;
  ch1monitor = _ch1monitor;
  ch2monitor = _ch2monitor;
  mode = modebox->currentItem();
  sync = syncbutton->isOn();
}

#endif // __loaded__mixerdialog_logic_cpp__
