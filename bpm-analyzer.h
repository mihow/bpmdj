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
#ifndef __loaded__bpm_analyzer_h__
#define __loaded__bpm_analyzer_h__
using namespace std;
#line 1 "bpm-analyzer.h++"
#include <qstring.h>
#include "bpmplay.h"
#include "analyzer.h"
#include "ui-bpmcounter.h"

typedef float8 fft_type;
class Player;

class BpmAnalyzerDialog : public QWidget, public Ui_CountDialog, 
			  public Analyzer
{
  Q_OBJECT
private:
  // fine scanning
  unsigned1 * audio;
  signed4   audiosize;
  unsigned4   audiorate;
  unsigned4   startbpm, stopbpm;
  unsigned4   startshift, stopshift;
  signed4   bufsiz;
  // progress indicator fields
  signed4    reading_progress;
  signed4    processing_progress;
  // tapping fields
  signed4    tapcount;
  signed4    starttime;
  // FFT fields
  fft_type * freq;
  fft_type * peak_bpm;
  fft_type * peak_energy;
  signed4  * peak_fit;
  signed4    windowsize;
  signed4    peaks;
  void set_measured_period(QString technique, signed4 period, 
			   bool update_on_disk=true);
  // Weighted envelope and autocorrelation
  void          wec();
  // Envelope FFT's
  void          enveloppe_spectrum();
  void          autocorrelate_spectrum();
  // FFT guidance routines
  void fft();        // do a quick FFT to obtain a set of 'hints'
  void fft_draw(QPainter &p, signed4 xs, signed4 ys, 
			 signed4 shifter, float8 bpm_divisor);
  void autocorrelate_draw(QPainter &p, signed4 xs, signed4 ys, signed4 shifter);
  // finding the error fit of a curve
  unsigned4 phasefit(unsigned4 i);
  unsigned4 phasefit(unsigned4 i, unsigned4 clip);
  void rayshoot_scan();
  void peak_scan();                    // scan based on FFT-peaks
  void readAudio();                    // reads the file in memory
  /**
   * Reads the file in memory divided by blocks
   */
  void readAudioBlock(signed4 blocksize);  
private:
  void status(QString text);
  void set_labels();
  void updateReadingProgress(signed4);
  void updateProcessingProgress(signed4);
  void updateInfo();
public:
  void setBpmBounds(signed4 start, signed4 stop);
  void getMd5();     // retrieves MD5 sum
  void writeAudio(); // writes audio to disk
  void analyze();
  void rangeCheck();
  void removeRaw();
  Player *  player;
  BpmAnalyzerDialog(QWidget*parent=0);
  virtual void started();
  virtual void stop();
  virtual void stopped();
public slots:
  virtual void startStopButton();
  virtual void finish();
  virtual void tap();
  virtual void reset();
};
#endif // __loaded__bpm_analyzer_h__
