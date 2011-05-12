/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__log_viewer_h__
#define __loaded__log_viewer_h__
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
#endif // __loaded__log_viewer_h__
