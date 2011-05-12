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

#include <unistd.h>
#include <qinputdialog.h>
#include <math.h>
#include <qmessagebox.h>
#include <qlcdnumber.h>
#include <ctype.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qtabwidget.h>
#include <assert.h>
#include <stdlib.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qprogressbar.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlistbox.h>
#include <qframe.h>
#include <libgen.h>
#include "version.h"
#include "kbpm-dj.h"
#include "analyzers-manager.h"
#include "analyzers-progress.h"
#include "common.h"
#include "memory.h"

#define ID 0
#define HOST 1
#define DONE 2
#define FAILED 3
#define RUNNING 4
#define COMMAND 5

// Processes per host
#define PPH 1
#define LIMIT (480 * PPH)

AnalyzersProgressLogic::~AnalyzersProgressLogic()
{
  delete processManager;
}

AnalyzersProgressLogic::AnalyzersProgressLogic(int i, QWidget * parent, const QString name) :
  AnalyzersProgress(parent,name)
{
  processManager = new AnalyzersManager(i*PPH,this);
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
  timer->start(1000);
  total = done = started = 0;
};

void AnalyzersProgressLogic::start()
{
  QListViewItemIterator it(hosts);
  while(it.current())
    {
      QListViewItem * i = it.current();
      startAnother(atoi(i->text(ID)));
      ++it;
    }
  bar->setProgress(done,total);
  started=time(NULL)-1;
}

void AnalyzersProgressLogic::startAnother(int id)
{
  // pop pending item if there is one
  QString tostart = pending->text(0);
  if (tostart.isEmpty() || tostart.isNull()) return;
  pending->removeItem(0);
  // find host and increase counter
  QListViewItem * i = hosts->findItem(QString::number(id),ID);
  assert(i);
  QString nr = i->text(DONE);
  QString host = i->text(HOST);
  i->setText(DONE,QString::number(atol(nr)+1));
  // create suitable command
  if (host!="localhost")
    tostart.replace("kbpm-play ","kbpm-play --remote "+host+" ");
  processManager->start(id,(const char*)tostart);
  QString toshow = tostart;
  toshow = tostart.right(tostart.length()-tostart.find("index"));
  toshow.replace("index/","\"");
  i->setText(COMMAND,toshow);
  i->setText(RUNNING,"0");
  // update the view
  bar->setProgress(++done);
  int secs = (time(NULL)-started)*(total-done)/done;
  int min = secs / 60;
  secs %= 60;
  int hours = min / 60;
  min %=60;
  eta->setText("Eta in "+ 
	       QString::number(hours)+" hours "+
	       QString::number(min)+" min "+
	       QString::number(secs)+" secs");
  pending_label->setText("Pending "+QString::number(total-done));
  speed->setText(QString("Speed ")+QString::number(done*60/(time(NULL)-started)));
}

void AnalyzersProgressLogic::addHost(int id, QString host)
{
  for(int i = 0 ; i <  PPH ; i++)
    new QListViewItem(hosts,QString::number(i + id * PPH),host,"0","0");
}

void AnalyzersProgressLogic::addLine(QString line)
{
  if (line.contains("kbpm-play "))
    {
      pending->insertItem(line);
      total++;
    };
}

void AnalyzersProgressLogic::timerTick()
{
  processManager->checkSignals();
  QListViewItemIterator it(hosts);
  while(it.current())
    {
      QListViewItem *i = it.current();
      long nr = atol(i->text(RUNNING)) + 1;
      if (nr<=LIMIT+1) 
	i->setText(RUNNING,QString::number(nr));
      if (nr == LIMIT) 
	{
	  nr = atol(i->text(FAILED)) + 1;
	  i->setText(FAILED,QString::number(nr));
	  startAnother(atol(i->text(0)));
	}
      it++;
    }
}

