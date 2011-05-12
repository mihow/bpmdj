/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
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

#ifndef BEATGRAPH_ANALYZER_H
#define BEATGRAPH_ANALYZER_H

#include <qpainter.h>
#include "songplayer.h"
#include "common.h"
#include "beatgraph-analyzer.h"
#include "basic-types.h"

// The define FILTER can be used to add the filter tab
// The define RYTHM can be used to insert the appropriate

typedef unsigned char compressed;
typedef float uncompressed;

class BeatGraphAnalyzerLogic : public BeatGraphDialog
{
  Q_OBJECT
private:
  void distribution(float *bank, long bank_size, float *distri, 
		    long distribution_size, float lower, float upper);
  void distribution(float *bank, long size, QPixmap &p, int slice);
  void showPatternVersion1();
  void showPatternVersion2();
  void showHaarVersion1();
  void showHaarVersion2();
  void showHaarVersion3();
  void settogrey();
  void rotate_slices();
public:
  long period;
  long audiosize;
  compressed * data;
  uncompressed * signed_data;
  BeatGraphAnalyzerLogic(bool showreaderprogress, SongPlayer*parent=0, const char * name=0, bool modal=FALSE, WFlags f=0);
  void readFile(bool showreaderprogress);
  void readFileSigned(bool showreaderprogress);
  // virtual void run();
public slots:
  virtual void showPattern();
  virtual void balanceChanged();
  virtual void slantChanged();
  virtual void setTempo();
  virtual void fatten();

#ifdef FILTER
  //  virtual void showFilter();
private:
  int     filtersize;
  double *filter;
  double  lo;
  double  hi;
  double  g_frequency_response(double freq);
  double  db_frequency_response(double freq, double clip);
  double  getGain(double freq);
  double  inverseDtft(double, double);
  double *doubleFilter(double *, int&);
  void    createFilter();
  void    filter_init();
public:
  virtual void filterChanged();
#endif

#ifdef RYTHM
  //  virtual void calculateRythmPattern();
  void calculateRythmPattern1();
  void calculateRythmPattern2();
  void calculateRythmPattern3();
  signed8 calculateRythmPattern2(QPainter & d, int demo_size_x, int demo_size_y, signed8 start, signed8 samples);
  void    calculateRythmPattern2(QPainter & d, int demo_size_x, int demo_size_y, signed8 start, signed8 samples, int depth);
  signed8 calculateRythmPattern3(QPainter & d, int demo_size_x, int demo_size_y, signed8 start=0, signed8 samples=-1);
#endif

};

#endif
