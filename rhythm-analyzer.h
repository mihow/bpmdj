/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__rhythm_analyzer_h__
#define __loaded__rhythm_analyzer_h__
using namespace std;
#line 1 "rhythm-analyzer.h++"
#include "ui-rhythm.h"
#include "analyzer.h"
#include "types.h"
#include "spectrum-type.h"

void fft_to_bark(float8 * in_r, int window_size, spectrum_type &out);

class SongPlayer;
class RhythmAnalyzer : 
  public QWidget, 
  public Ui::RhythmAnalyzer, 
  public Analyzer
{
  Q_OBJECT
public:
  RhythmAnalyzer(QWidget * parent=0);
public slots:
  virtual void calculateRhythmPattern2();
  virtual void analyze() { calculateRhythmPattern2();};
  virtual void on_calculate_clicked() { start(); };
};

void write_out_projection(long slice_size, unsigned4 *phases, 
			  int maximum_slice, const char * target);
#endif // __loaded__rhythm_analyzer_h__
