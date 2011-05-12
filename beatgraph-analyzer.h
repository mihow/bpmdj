/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __loaded__beatgraph_analyzer_h__
#define __loaded__beatgraph_analyzer_h__
using namespace std;
#line 1 "beatgraph-analyzer.h++"
#include <Qt/qpainter.h>
#include "common.h"
#include "ui-beatgraph.h"

typedef unsigned char compressed;
typedef float uncompressed;

class BeatGraphAnalyzer : public QWidget, public Ui::BeatGraph
{
  Q_OBJECT
private: 
  // the signed data is used as input into the energy and haar calculations
  signed4 audiosize;
  uncompressed * signed_data;
  void readFileSigned();
  // the period under which the beatgraph is being visualized
  signed4 period;
  void getTempo();
  bool check_visualisation_conditions(bool file_read=true);
  // energy calculation, storage & visualisation
  compressed   * data;
  void calculateEnergy();
  void showEnergyPattern();
  // haar calculation, storage & visualisation
  float * * bank;
  void calculateHaar();
  void showHaarPattern();
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
