/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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
#include "similarscanner.h"
#include "qsong.h"
#include "kbpm-dj.h"

extern "C"
{
#include "edit-distance.h"
}

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
  QListViewItem *selected = NameList->selectedItem();
  QString fullname = selected -> text(LIST_FULLSONGNAME);
  char fullcommand[2048];
  sprintf(fullcommand,"rm -- \"%s\"",(const char*)fullname);
  printf("Executing %s\n",fullcommand);
  system(fullcommand);
  getRidOff(fullname);
}

void SimilarScanner::ignore()
{
  QListViewItem * selected = NameList->selectedItem();
  QString fullname = selected -> text(LIST_FULLSONGNAME);
  printf("Ignoring %s\n",(const char*)fullname);
  getRidOff(fullname);
}

void SimilarScanner::goFetch()
{
  QListViewItem * selected = NameList -> selectedItem();
  QString fullname = selected -> text(LIST_FULLSONGNAME);
  goFetch(fullname);
  getRidOff(fullname);
}

void SimilarScanner::goFetch(QString fullname)
{
  char fullcommand[2048];
  QString newname = fullname;
  newname.replace(".mp3",".tofetch");
  sprintf(fullcommand,"mv -i -- \"%s\" \"%s\" ",(const char*)fullname,(const char*)newname);
  printf("Executing %s\n",fullcommand);
  system(fullcommand);
}

void SimilarScanner::checkfile(const QString pathname, const QString filename)
{ 
  QString stripped = filename.left(filename.length()-4);
  QString fullname = pathname + "/" + filename;
  currentFile->setText(stripped);
  findSimilarNames(stripped,fullname);
  if (!similarnames)
    goFetch(fullname);
};

void SimilarScanner::similarNameFound(QString name, QString similar, QString fullname, QString fullsimilar, int dist)
{
  similarnames=true;
  new QListViewItem(NameList,
		    name, similar,
		    QString::number(dist),
		    fullname, fullsimilar);
  /**
   * WVB -- a piece of code to decode the files
    if (!fullname.isEmpty())
    {
    system("mpg123 -0 -4 -t \""+fullname
    +"\" 2>/tmp/p; grep kbit /tmp/p; tail -n 1 /tmp/p");
    system("mpg123 -0 -4 -t \"./music/"+song->song_file
    +"\" 2>/tmp/p; grep kbit /tmp/p; tail -n 1 /tmp/p");
    printf("\n");
    }
  */
}

void SimilarScanner::findSimilarNames(QString text, QString fullname)
{
  QListView *songList = selector->songList;
  QListViewItemIterator it1(songList);
  similarnames=false;
  for(;it1.current();++it1)
    {
      app->processEvents();
      QSong * song = (QSong*)it1.current();
      if (song)
	{
	  const char * t = song->song_title;
	  const char * a = song->song_author;
	  int val=ndist(t,a,text);
	  if ( val < (text.length()/10)+3)
	    similarNameFound(text,song->song_title+"["+song->song_author+"]",
			     fullname,song->song_file,
			     val);
	}
    }
}

SimilarScanner::SimilarScanner(SongSelectorLogic* sroot) :  
  DirectoryScanner(".mp3")
{ 
  selector = sroot; 
  show();
};

void SimilarScanner::scan(const QString dirname) 
{ 
  dist_init();
  root->setText(dirname);
  DirectoryScanner::scan(dirname,dirname); 
  dist_done();
};
