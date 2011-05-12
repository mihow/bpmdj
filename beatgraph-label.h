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
#include <Qt/qtimer.h>
#include <Qt/qlabel.h>
#include <Qt/qpainter.h>
#include "common.h"

/**
 * The port from Qt3 to Qt4 is a bit tricky here. First: the new painter system does not allow us to draw outside
 * a paintevent message, so we need to fake these. Secondly: the double buffering makes the xor operation effectively
 * useless. Third: removing the old line with a small redraw will redraw the entire bitmap regardless. 
 * These 3 reasons forced me to redraw the content of the window myself by updating the resized pixmap.
 * The result is that drawCursor will always update the image in a way that only leaves 1 line on the image. Once the 
 * image is updated we either request two paint operations (if the lines are far apart), or 1 merged operation if they
 * are close to each other.
 */
class BeatGraphLabel: public QWidget
{
  Q_OBJECT
private:
  /**
   * Will draw a yellow vertical line at position ox on the resized image.
   * The ox coordinate is given in the _original_ image coordinates. This
   * makes it possible to refer to measure x and draw the ruler at the 
   * right position in the resized image. The output parameters x1 and x2
   * contain the boundaries that require redrawing on the widget.
   */
  void xorLine(int ox, int& x1, int& x2);
protected:
  /**
   * If there is no image set, an empty area is drawn. If the cues changed
   * a total redraw is required, otherwise the required areas are copied 
   * from the resized image onto the widget.
   */
  virtual void paintEvent(QPaintEvent *);
  /**
   * When a resize of the widget occur, we recalculate the resized image and redraw
   * the cursor
   */
  virtual void resizeEvent(QResizeEvent * event);
  /**
   * The original image as produced in the beatgraph analyzer is placed here. Depending on 
   * the size of the target window, we resize it to something smaller/or larger.
   */
  QImage original;
  /**
   * The resized image which fits the width() and height() of the widget perfectly.
   */
  QImage resized;
  /**
   * The current ruler position; expressed in the internal image coordinates
   */
  int ruler_x; 
  /**
   * Will draw the different cues on the resized image
   */
  void drawCueText();
  /**
   * Will move the ruler and report back (through the 4 output parametes) which 
   * columns must be redrawn.
   */
  void moveRuler(int &x1, int &x2, int &x3, int &x4);
public:
  /**
   * The timer used to update the image if necessary. This timer will be connected
   * to the drawCursor slot.
   */
  QTimer timer;
  /**
   * Remembers how many audiosamples are in one measure. 
   * This makes it possible to calculate the current measure
   * based on the current songposition ::x
   */
  unsigned8 samples_per_column;
  /**
   * Creates a beatgraphlabel to be used in BpmPlay.
   */
  BeatGraphLabel(QWidget * parent, const char * name = 0);
  /**
   * Called to start the cursor movements. It will start the timer.
   */
  void activate() {timer.start(333);};
  /**
   * Called to stop the cursor movements. It stops the timer.
   */
  void deactivate() { timer.stop(); };
  virtual ~BeatGraphLabel();
  bool cues_changed;
public slots:
  /**
   * will move the ruler with moveruler() and then decide which 
   * areas must be repainted. If the old and new ruler were
   * close to each other, one repaint event is scheduler. If not 
   * two different repaints are called for.
   */
  virtual void drawCursor();
  /**
   * once the pixamp is set it remains the property of the label
   */
  virtual void setImage(QImage, unsigned8 samplespercol);
};

#endif
