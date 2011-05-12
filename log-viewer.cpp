/****
 BpmDj v4.2 beta: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__log_viewer_cpp__
#define __loaded__log_viewer_cpp__
using namespace std;
#line 1 "log-viewer.c++"
#include <cstdio>
#include <qstringlist.h>
#include <qlistview.h>
#include <qstring.h>
#include <qtextedit.h>
#include "log-viewer.h"
#include "constants.h"

class LogItem: public QListWidgetItem
{
  QString logtext;
public:
  LogItem(QListWidget* parent, QString dirname, QString logfilename);
  void setLogText(QStringList t)
  {
    logtext=t.join("");
  }
  QString getLogText() 
  {
    return logtext;
  };
};

QString wrap_color(QString t, QString b, QString color)
{
  if (!t.startsWith(b)) return t;
  t.remove(0,b.length());
  return QString("<font color=\"#")+color+"\">"+t+"</font>";
}

LogItem::LogItem(QListWidget* parent, QString dirname, QString logfilename) :
  QListWidgetItem(logfilename,parent)
{
  QStringList logtext;
  QString fullname = dirname+slash+logfilename;
  FILE * fp = fopen(fullname.toAscii().data(),"rb");
  if (!fp)
    {
      setText(logfilename);
      logtext.append(QString("<b>Impossible to open file ")+fullname+"</b>");
      setLogText(logtext);
      return;
    }
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int lineno = 0;
  while ((read = getline(&line, &len, fp)) != -1)
    {
      QString t(line);
      if (++lineno==1)
	{
	  t.replace("\n","");
	  setText(t);
	  logtext.append(QString("<b>Name: "+t+"</b>"));
	}
      else if (t.startsWith("BpmDj Player v") ||
	       t.startsWith("This software is distributed") ||
	       t.startsWith("---------------------------"))
	{
	  continue;
	}
      else
	{
	  t = wrap_color(t,"Information: ","0000ff");
	  t = wrap_color(t,"Error: ","ff0000");
	  t = wrap_color(t,"Debug: ","777777");
	  t = wrap_color(t,"Remote: ","aa007f");
	  logtext.append(t);
	}
      logtext.append("<br>");
    }
  if (line)
    free(line);
  fclose(fp);
  setLogText(logtext);
}

LogViewer::LogViewer(QWidget*parent): 
  QDialog(parent), 
  DirectoryScanner("/tmp",".bpmdj.log",false)
{
  setupUi(this);
  scan();
}

void LogViewer::checkfile(const QString fullname, const QString filename)
{
  new LogItem(list,fullname,filename);
}

void LogViewer::on_list_currentItemChanged(QListWidgetItem* i, 
					   QListWidgetItem* p)
{
  if (!i) return;
  LogItem *cast = (LogItem*)i;
  text->setText(cast->getLogText());
}

void LogViewer::goTo(QString name)
{
  QList<QListWidgetItem*> lst = list->findItems(name,0);
  if (lst.count()>0)
    {
      lst.first()->setSelected(true);
      list->setCurrentItem(lst.first());
    }
}
#endif // __loaded__log_viewer_cpp__
