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
using namespace std;
#line 1 "log-viewer.c++"
#include <qstringlist.h>
#include <qlistview.h>
#include <qstring.h>
#include <qtextedit.h>
#include "log-viewer.h"
#include "constants.h"

class LogItem: public QListWidgetItem
{
  QStringList logtext;
public:
  LogItem(QListWidget* parent, QString dirname, QString logfilename);
  QString getLogText() 
  {
    return logtext.join("");
  };
};

QString wrap_color(QString t, QString b, QString color)
{
  if (!t.startsWith(b)) return t;
  t.remove(0,b.length());
  return QString("<font color=\"#")+color+"\">"+t+"</font>";
}

LogItem::LogItem(QListWidget* parent, QString dirname, QString logfilename) :
  QListWidgetItem(logfilename,parent), 
  logtext()
{
  QString fullname = dirname+slash+logfilename;
  FILE * fp = fopen(fullname,"rb");
  if (!fp)
    {
      setText(logfilename);
      logtext.append(QString("<b>Impossible to open file ")+fullname+"</b>");
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

void LogViewer::on_list_currentItemChanged(QListWidgetItem* i, QListWidgetItem* p)
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
