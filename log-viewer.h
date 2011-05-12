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
#ifndef __BPMDJ___LOG_VIEWER_H__
#define __BPMDJ___LOG_VIEWER_H__
using namespace std;
#line 1 "log-viewer.h++"
#include "ui-logs.h"
#include "dirscanner.h"

class LogViewer:
  public QDialog,
  public Ui::LogViewer, 
  protected DirectoryScanner
{
  Q_OBJECT
protected:
  virtual void checkfile(const QString fullname, const QString filename);
public:
  LogViewer(QWidget*parent=0);
  void addLog(const char* name, const char* file);
  void goTo(QString name);
public slots:
  virtual void on_list_currentItemChanged(QListWidgetItem *,QListWidgetItem *);
};
#endif
