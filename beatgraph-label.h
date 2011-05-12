/****
 BpmDj: Free Dj Tools
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
#ifndef __BPMDJ___BEATGRAPH_LABEL_H__
#define __BPMDJ___BEATGRAPH_LABEL_H__
using namespace std;
#line 1 "beatgraph-label.h++"
#include <qtimer.h>
#include <qlabel.h>
#include <qpainter.h>
#include "common.h"

class BeatGraphLabel: public QLabel
{
  Q_OBJECT
protected:
  virtual void paintEvent(QPaintEvent *);
public:
  int oldpos;  // the current ruler position, slightly misnamed
  QTimer timer;
  unsigned8 samples_per_column;
  void drawRuler();
  void drawCueText();
  BeatGraphLabel(QWidget * parent, const char * name = 0);
  void activate() {timer.start(333);};
  void deactivate() { timer.stop();};
  virtual ~BeatGraphLabel();
public slots:
  virtual void drawCursor();
  virtual void setPixmap ( const QPixmap &, unsigned8 samplespercol);
};

#endif
