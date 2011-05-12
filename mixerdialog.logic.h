/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__mixerdialog_logic_h__
#define __loaded__mixerdialog_logic_h__
using namespace std;
#line 1 "mixerdialog.logic.h++"
#include <qtimer.h>
#include <q3button.h>
#include "ui-mixerdialog.h"

#define dspmixmaxmult 128
#define dspmixdoublemult (128*2)

class MixerDialogLogic : 
  public MixerDialog
{
   Q_OBJECT
 public:
   int ch1main;
   int ch2main;
   int ch1monitor;
   int ch2monitor;
   int mode;
   int sync;
   bool stop;
   bool stopped;
   MixerDialogLogic(QWidget*parent=0,const char*name=0, 
		    bool modal=FALSE, Qt::WindowFlags f=0);
 public slots:
   virtual void volumesChanged();
   virtual void accept();
   virtual void reject();
};
#endif // __loaded__mixerdialog_logic_h__
