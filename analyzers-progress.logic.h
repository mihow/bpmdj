/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef ANALYZERSBATCH_LOGIC_H
#define ANALYZERSBATCH_LOGIC_H

#include <qtimer.h>
#include <qlistview.h>
#include "analyzers-progress.h"
#include "analyzers-manager.h"
#include "basic-process-manager.h"

class AnalyzersManager;

class AnalyzersProgressLogic: 
public AnalyzersProgress, public ProcessChanged
{
    Q_OBJECT
  private:
    AnalyzersManager *processManager;
    QTimer *timer;
    time_t started;
    int total;
    int done;
    
  public:
    AnalyzersProgressLogic(int i, QWidget*parent=0, const QString name=0);
    virtual ~AnalyzersProgressLogic();
    virtual void startAnother(int id);
    virtual void addHost(int id, QString host);
    virtual void addLine(QString line);
    virtual void start();
  public slots:
    virtual void timerTick();
};

#endif
