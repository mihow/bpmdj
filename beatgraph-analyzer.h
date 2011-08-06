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
#ifndef __loaded__beatgraph_analyzer_h__
#define __loaded__beatgraph_analyzer_h__
using namespace std;
#include <Qt/qpainter.h>
#include "common.h"
#include "ui-beatgraph.h"

typedef unsigned char compressed;
typedef float4 uncompressed;

class BeatGraphAnalyzer : public QWidget, public Ui::BeatGraph
{
  Q_OBJECT
private: 
  // the signed data is used as input into the energy and Haar calculations
  signed4 audiosize;
  uncompressed * signed_data;  // contains audiosize/COLLAPSED samples
  void readFileSigned();
  // the period under which the beatgraph is being visualized
  signed4 period_diskrate;
  void getTempo();
  bool check_visualisation_conditions(bool file_read=true);
  // energy calculation, storage & visualization
  compressed  * data;
  void calculateEnergy();
  void showEnergyPattern();
  // Haar calculation, storage & visualization
  float4 ** bank;
  unsigned1* f1;
  void calculateHaar();
  void showHaarPattern();

  compressed * hue;
  compressed * sat;
  void calculateF1();
  void showF1Pattern();
public:
  BeatGraphAnalyzer(QWidget * parent = 0, const char * name=0);
  void readFileSigned(bool showreaderprogress);
signals:
  void targetTempo();
  void normalTempo();
public slots:
  virtual void activate();
  virtual void deactivate();
  virtual void showPattern();
  virtual void slantChanged();
  virtual void setTempo();
  virtual void changeVisualisation();
  virtual void cuesChanged();
};
#endif // __loaded__beatgraph_analyzer_h__
