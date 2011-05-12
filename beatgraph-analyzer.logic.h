/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include "beatgraph-widget.h"

typedef unsigned char compressed;
typedef float uncompressed;

class BeatGraphAnalyzer : public BeatGraphWidget
{
    Q_OBJECT
 private: 
    // the signed data is used as input into the energy and haar calculations
    long audiosize;
    uncompressed * signed_data;
    void readFileSigned();
    // the period under which the beatgraph is being visualized
    long period;
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
    virtual void showPattern();
    virtual void slantChanged();
    virtual void setTempo();
    virtual void changeVisualisation();
};

#endif
