/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

#include <qlabel.h>
#include <qstring.h>
#include <qlistview.h>
#include "qvectorview.h"
#include "similarscanner.h"
#include "qsong.h"
#include "kbpm-dj.h"
#include "edit-distance.h"
#include "scripts.h"

#define LIST_SONGNAME 0
#define LIST_SIMILARNAME 1
#define LIST_DISTANCE 2
#define LIST_FULLSONGNAME 3
#define LIST_FULLSIMILARNAME 4

void SimilarScanner::getRidOff(QString fullname)
{
  QListViewItemIterator it(NameList);
  for(;it.current();)
    {
      QListViewItem * item = it.current();
      if (item->text(LIST_FULLSONGNAME)==fullname)
	delete item;
      else
	++it;
    }
}

void SimilarScanner::alreadyHave()
{
  bool reset = true;
  while(reset)
    {
      reset = false;
      QListViewItemIterator it(NameList);
      while(it.current())
	{
	  QListViewItem * selected = (QListViewItem*)it.current();
	  if (selected->isSelected())
	    {
	      QString fullname = selected -> text(LIST_FULLSONGNAME);
	      vexecute(RM" \"%s\"",(const char*)fullname);
	      getRidOff(fullname);
	      reset=true;
	      break;
	    }
	  ++it;
	}
    }
}

void SimilarScanner::ignore()
{
  QListViewItemIterator it(NameList);
  while(it.current())
    {
      QListViewItem * selected = (QListViewItem*)it.current();
      if (selected->isSelected())
	{
	  QString fullname = selected -> text(LIST_FULLSONGNAME);
	  printf("Ignoring %s\n",(const char*)fullname);
	  getRidOff(fullname);
	}
      ++it;
    }
}

void SimilarScanner::goFetch()
{
  QListViewItemIterator it(NameList);
  while(it.current())
    {
      QListViewItem * selected = (QListViewItem*)it.current();
      if (selected->isSelected())
	{
	  QString fullname = selected -> text(LIST_FULLSONGNAME);
	  goFetch(fullname);
	  getRidOff(fullname);
	}
      ++it;
    }
}

void SimilarScanner::goFetch(QString fullname)
{
  QString newname = fullname+".tofetch";
  vexecute(MV" \"%s\" \"%s\" ",(const char*)fullname,(const char*)newname);
}

void SimilarScanner::checkfile(const QString pathname, const QString filename)
{ 
  if (conformingFilesOnly->isChecked())
    if (!goodName(filename))
      return ;
  QString stripped = filename.left(filename.length()-4);
  QString fullname = pathname + "/" + filename;
  currentFile->setText(stripped);
  findSimilarNames(stripped,fullname);
  if (!similarnames)
    if (autoFetch->isChecked())
      goFetch(fullname);
};

void SimilarScanner::similarNameFound(QString name, QString similar, QString fullname, QString fullsimilar, int dist)
{
  similarnames = true;
  new QListViewItem(NameList,
		    name, similar,
		    QString::number(dist),
		    fullname, fullsimilar);
}

void SimilarScanner::findSimilarNames(QString text, QString fullname)
{
  QVectorView *songList = selector->songList;
  similarnames = false;
  char exact[1024];
  for(int it1 = 0 ; it1 < QSong::song_count ; it1 ++)
    {
      app->processEvents();
      Song * song = QSong::songEssence(it1);
      if (song)
	{
	  const char * t = song->title;
	  const char * a = song->author;
	  unsigned int val=ndist(t,a,text);
	  if ( val < (text.length()/10)+3)
	    {
	      sprintf(exact,"%s[%s]",t,a);
	      similarNameFound(text,song->title+"["+song->author+"]",
			       fullname,song->file,
			       (strcasecmp(exact,(const char*)text)==0) ? 
			       ((strcmp(a,"")==0) ? -1 : -2) : val);
	    }
	}
    }
}

SimilarScanner::SimilarScanner(SongSelectorLogic* sroot) :  
  DirectoryScanner(NULL)
{ 
  selector = sroot; 
  show();
};

bool SimilarScanner::matchextension(const QString filename)
{
  return goodExtension(filename);
}

void SimilarScanner::scan(const QString dirname) 
{ 
  dist_init();
  root->setText(dirname);
  DirectoryScanner::scan(dirname,dirname); 
  dist_done();
};

void SimilarScanner::setRoot(QString w)
{
  root->setText(w);
}

void SimilarScanner::startStop()
{
  startStopButton->setEnabled(false);
  scan(root->text());
  startStopButton->setEnabled(true);
}

