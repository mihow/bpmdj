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
#ifndef __loaded__bpm_analyzer_dialog_h__
#define __loaded__bpm_analyzer_dialog_h__
using namespace std;
#include <cstdio>
#include <qstring.h>
#include "bpmplay.h"
#include "ui-bpmcounter.h"
#include "analyzer-progress-qt.h"
#include "bpm-analyzer-impl.h"
#include "analyzer-worker.h"

class BpmAnalyzerProgress: public QObject, public QAnalyzerProgress, public AnalyzerProgress
{
  Q_OBJECT
  private:
  int last_progress;
public:
  BpmAnalyzerProgress(): last_progress(-1)
  {
  };
  virtual void status(const char* name)
  {
    emit(newStatusText(QString(name)));
  }
  virtual void progress(int a, int t)
  {
    int pct=a*100/t;
    if (pct!=last_progress)
      {
	emit(newProgress(pct));
	last_progress=pct;
      }
  }
  virtual void finished()
  {
    emit(finishedAnalyzing());
  }
signals:
  void newProgress(int t);
  void newStatusText(QString txt);
  void finishedAnalyzing();
};

class BpmAnalyzerDialog : public QWidget, public Ui_CountDialog
{
  Q_OBJECT
private:
  // the audio being analyzed
  stereo_sample2* audio;
  unsigned4 map_length;
  // the analyzer being run
  BpmAnalyzer bpmanalimp;
  // the callback that resides permant here
  BpmAnalyzerProgress progress;
  AnalyzerWorker worker;
  unsigned4 startbpm, stopbpm;
  // tapping fields
  signed4    tapcount;
  signed4    starttime;
private:
  void set_measured_period_diskrate(signed4 p,bool update_on_disk);
  void retrieve_image();
public:
  void setBpmBounds(signed4 start, signed4 stop);
  BpmAnalyzerDialog(QWidget*parent=0);
  virtual void start();
  virtual void terminate();
public slots:
  virtual void updateProgress(int);
  virtual void status(QString text);
  virtual void startStopButton();
  virtual void finished();
  virtual void tap();
  virtual void reset();
};
#endif // __loaded__bpm_analyzer_dialog_h__
