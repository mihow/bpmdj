/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__renamer_cpp__
#define __loaded__renamer_cpp__
using namespace std;
#line 1 "renamer.c++"
#include <assert.h>
#include <cstdio>
#include <ctype.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <stdlib.h>
#include <qfiledialog.h>
#include <stdio.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include "renamer.h"
#include "selector.h"
#include "scripts.h"
#include "capacity.h"
#include "song-iterator.h"
#include "info.h"
#include "qt-helpers.h"

RenamerLogic::RenamerLogic(QWidget*parent, RenamerChangesFilename *rcf) :
  QDialog(parent,Qt::Window),
  DirectoryScanner(""),
  inform(rcf)
{
  setupUi(this);
}

RenamerLogic::~RenamerLogic()
{
  if (inform) 
    delete inform;
}

/**
 * Since Qt4 a modification to a text item directly resorts
 * the surrounding item list, the interator looses completely 
 * track. This double iteration deals with that.
 */
#define FOREACH(operation) {					     \
  map<QTreeWidgetItem*,QString> newtexts;			     \
  QTreeWidgetItemIterator it(NameList);				     \
  for(;*it;++it) {						     \
    QTreeWidgetItem * item = *it;				     \
    if (item->isSelected())					     \
      newtexts[item]=operation(item->text(0));			     \
  }								     \
  for(map<QTreeWidgetItem*,QString>::iterator it=newtexts.begin();   \
      it!=newtexts.end(); it++)					     \
    it->first->setText(0,it->second);				     \
  }

void RenamerLogic::checkfile(const QString  pathname, const QString  filename)
{
  QString fullname = pathname+"/"+filename;
  add(filename,fullname);
}

bool RenamerLogic::matchextension(const QString  filename)
{
  return goodExtension(filename) 
    || filename.contains(".idx",Qt::CaseInsensitive);
}

void RenamerLogic::add(const QString name, const QString pos)
{
  // check whether the filename is too good
  if (goodName(name)) return;
  // so it is an incorrect filename which does not contain correct information
  QStringList content;
  content << name << name << pos;
  new QTreeWidgetItem(NameList,content);
}

void RenamerLogic::add(Song* s)
{
  if (!s) return;
  QString v=s->getDisplayTitle();
  QStringList content;
  content << v
	  << v
	  << s->get_storedin();
  new QTreeWidgetItem(NameList,content);
}

QString RenamerLogic::smallCapsInWord(QString in)
{
  char* out = strdup(in.toAscii().data());
   int i = 0;
   bool prevupper = false;
   while(out[i])
     {
	if (isupper(out[i]))
	  {
	     if (prevupper)
	       out[i]=tolower(out[i]);
	     prevupper=true;
	  }
	else
	  {
	     prevupper=false;
	  }
	i++;
     }
   return QString(out);
}

QString RenamerLogic::capitalizeAfterSpace(QString in)
{
   char* out = strdup(in.toAscii().data());
   int i = 0;
   bool prevspace = true;
   while(out[i])
     {
       if (islower(out[i]))
	 {
	   if (prevspace)
	     out[i]=toupper(out[i]);
	 }
       prevspace = (out[i]==' ') 
	 || (out[i]=='_') 
	 || (out[i]=='[');
       i++;
     }
   return QString(out);
}

QString RenamerLogic::removeSpaces(QString in)
{
  char * out = strdup(in.toAscii().data());
   int i = 0;
   int j = 0;
   while(out[i])
     {
	if (out[i] != ' ')
	  {
	     out[j] = out[i];
	     j++;
	  }
	i++;
     }
   out[j]=out[i];
   return QString(out);
}

QString RenamerLogic::betweenBracesIsTrash(QString in)
{
  char * out = strdup(in.toAscii().data());
  int i = 0;
  int j = 0;
  bool removing = false;
  while(out[i])
    {
      if (removing)
	{
	  if (out[i]==')')
	    removing=false;
	}
      else
	{
	  if (out[i]=='(')
	    removing=true;
	  else 
	    {
	      out[j++]=out[i];
	    }
	}
      i++;
    }
  out[j]=out[i];
  return QString(out);
}

QString RenamerLogic::replaceUnderscores(QString in)
{
   char * out = strdup(in.toAscii().data());
   int i = 0;
   while(out[i])
     {
       if (out[i] == '_')
	 out[i]=' ';
       i++;
     }
   return QString(out);
}

QString RenamerLogic::removeSpecials(QString in)
{
  char * out = strdup(in.toAscii().data());
  int i = 0;
  int j = 0;
  while(out[i])
    {
      if (isalnum(out[i]) || out[i]=='&'
	  || out[i]=='(' || out[i]==')'
	  || out[i]=='[' || out[i]==']'
	  || out[i]=='{' || out[i]=='}' 
	  || (out[i]=='.' && i == (signed)in.length()-4))
	{
	  out[j]=out[i];
	  j++;
	}
      i++;
    }
  out[j]=out[i];
  return QString(out);
}

QString RenamerLogic::removeFirstSpecials(QString in)
{
   char * out = strdup(in.toAscii().data());
   int newstart = 0;
   while(out[newstart])
     {
       if (isalpha(out[newstart]))
	 break;
       newstart++;
     }
   return in.right(in.length()-newstart);
}

QString RenamerLogic::replaceSubString(QString in)
{
   QString key = SubString->text();
   QString replace = ReplaceString->text();
   int pos = in.indexOf(key);
   if (pos!=-1)
     return in.replace(pos,key.length(),replace);
   return in;
}

QString RenamerLogic::deleteSubString(QString txt)
{
  QString key = SubString->text();
  int i=txt.indexOf(key);
  if (i>=0)
    txt.replace(i,key.length(),EMPTY);
  return txt;
}

QString RenamerLogic::beforeMinusIsAuthor(QString in)
{
  if (in.contains('-')!=1) 
    return in;
  int pos = in.indexOf('-');
  QString author = in.left(pos);
  QString out = in.right(in.length()-pos-1);
  if (in[in.length()-4]!='.')
    return out.append("["+author+"]");
  else
    return out.insert(out.length()-4,"["+author+"]");
}

QString RenamerLogic::fixExtention(QString in)
{
  QString ext = in.right(4).toLower();
  if (goodExtension(ext) || ext==".idx")
    return in.replace(in.length()-4,4,ext);
  return in;
}

QString RenamerLogic::removeFirstChar(QString in)
{
  return in.right(in.length()-1);
}

void RenamerLogic::keySelectionIsAuthor() 
{
  printf("Not Implemented Yet\n");
}

QString RenamerLogic::subStringIsAuthor(QString txt)
{
  // check extension
  if (txt[txt.length()-4]=='.')
    {
      QString key = "["+SubString->text()+"]"+txt.right(4);
      txt = txt.left(txt.length()-4);
      txt = txt.append(key);
      return txt;
    }
  else
    {
      QString key = "["+SubString->text()+"]";
      txt = txt.append(key);
      return txt;
    }
}

QString RenamerLogic::subStringIsMix(QString txt)
{
  // we must place the substring before the '[' if there is one,
  // otherwise we must place it before the last ., if there is one
  int posbracket = txt.indexOf("[");
  if (posbracket<0)
    posbracket = txt.lastIndexOf(".");
  if (posbracket>=0)
    txt.insert(posbracket,QString("{")+SubString->text()+"}");
  return txt;
}

QString RenamerLogic::betweenBracesIsMix(QString txt)
{
  return txt.replace("(","{").replace(")","}");
}

void RenamerLogic::replaceSubString()
  FOREACH(replaceSubString);

void RenamerLogic::capitalizeAfterSpace()
  FOREACH(capitalizeAfterSpace);

void RenamerLogic::removeSpaces() {
  FOREACH(removeSpaces);
  changeSelection();}

void RenamerLogic::removeSpecials() {
  FOREACH(removeSpecials);
  changeSelection();}

void RenamerLogic::removeFirstChar()
  FOREACH(removeFirstChar);

void RenamerLogic::deleteSubString()
  FOREACH(deleteSubString);

void RenamerLogic::smallCapsInWords()
  FOREACH(smallCapsInWord);

void RenamerLogic::subStringIsAuthor()
  FOREACH(subStringIsAuthor);

void RenamerLogic::beforeMinusIsAuthor()
  FOREACH(beforeMinusIsAuthor);

void RenamerLogic::betweenBracesIsTrash() 
  FOREACH(betweenBracesIsTrash);

void RenamerLogic::betweenBracesIsMix() 
  FOREACH(betweenBracesIsMix);

void RenamerLogic::substringIsMix() 
  FOREACH(subStringIsMix);

void RenamerLogic::removeFirstSpecials() 
  FOREACH(removeFirstSpecials);

void RenamerLogic::replaceUnderscores() 
  FOREACH(replaceUnderscores);

void RenamerLogic::fixExtention() 
  FOREACH(fixExtention);

void RenamerLogic::changeSelection()
{
  // find smallest, this will be our key to find 
  // the greatest common substring in
  QString key;
  int l = -1;
  QTreeWidgetItemIterator it(NameList);
  for(;*it;++it)
    {
      QTreeWidgetItem * item = *it;
       if (item->isSelected())
	 {
	   QString txt = item->text(0);
	   if (l==-1 || (signed)txt.length()<l)
	     {
	       l=txt.length();
	       key=txt;
	     }
	 }
     }
   // now lets look for a matching substring
   bool found = false;
   QString gcs;
   for(int size = key.length(); size>0 && !found; size--)
     {
       for(int pos = key.length()-size; pos>=0 && !found; pos--) 
	 {
	   QTreeWidgetItemIterator it(NameList);
	   found = true;
	   gcs = key.mid(pos,size);
	   for(;*it && found;++it)
	     {
	       QTreeWidgetItem * item = *it;
	       if (item->isSelected())
		 {
		   if (!item->text(0).contains(gcs))
		     found=false;
		 }
	     }
	 }
     }
   if (!found) 
     gcs="";
   SubString->setText(gcs);
}

void RenamerLogic::realizeSelection()
{
  set<QTreeWidgetItem*> todelete;
  stdTreeWidgetIterator it(NameList);
ITERATE_OVER(it)

    QTreeWidgetItem * item = it.val();
    if (item->isSelected())
      {
	QString to=item->text(0);
	QString from=item->text(1);
	QString location=item->text(2);
	assert(inform);
	if (inform->change(from,to,location))
	  todelete.insert(item);
      }
    }
  
  /**
   * Delete this part of the selection
   */
  for(set<QTreeWidgetItem*>::iterator it2=todelete.begin();
      it2!=todelete.end(); it2++)
    delete(*it2);
}

void RenamerLogic::start_dialog()
{
  if (NameList->topLevelItemCount())
    {
      NameList->resizeColumnToContents(0);
      NameList->resizeColumnToContents(1);
      NameList->resizeColumnToContents(2);
      NameList->sortItems(2,Qt::AscendingOrder);
      show();
    }
  else
    QMessageBox::information(this, 
     "Renamer",
     "There are no wrongly named entries");
}

void RenamerLogic::scan(const QString dirname)
{
  reset(dirname);
  DirectoryScanner::scan();
}

void RenamerLogic::ignoreSelection()
{
  QTreeWidgetItemIterator it(NameList);
  for(;*it;)
    {
      QTreeWidgetItem * item = *it;
      if (item->isSelected())
	delete item;
      else ++it;
    }
}

//---------------------------------------------------------
//   Updating filename of files on disk
//---------------------------------------------------------
class UpdateSongFilename: public RenamerChangesFilename
{
public:
  UpdateSongFilename()
  {
  };
  virtual bool change(QString from, QString to, QString location);
};

bool UpdateSongFilename::change(QString toreplace, QString replaceby,
				QString from)
{
  QString to = from;
  int pos = to.indexOf(toreplace);
  assert(pos>=0);
  to.replace(pos,toreplace.length(),replaceby);
  // if from = to don't do anything otherwise
  if (from == to) return true;
  if (exists(to))
    {
      // fix target filename...
      int trie = 1;
      QString nto;
      do
	{
	  trie++;
	  nto=to;
	  char nr[50];
	  sprintf(nr,"%d",trie);
	  nto.insert(nto.length()-4,nr);
	}
      while (exists(nto));
      to=nto;
    }
  Debug("Would move %s to %s",from.toAscii().data(),to.toAscii().data());
  start_mv(from,to);
  return exists(to);
};

//---------------------------------------------------------
//   Updating changing index files
//---------------------------------------------------------
class UpdateIndexedSong: public RenamerChangesFilename
{
private:
  DataBase* database;
  SongSelectorLogic * selector;
public:
  UpdateIndexedSong(SongSelectorLogic * l);
  virtual bool change(QString from, QString to, QString location);
};

UpdateIndexedSong::UpdateIndexedSong(SongSelectorLogic * l)
{
  selector = l;
  assert(selector);
  database = l->database;
  assert(database);
}

bool UpdateIndexedSong::change(QString from, QString to, QString location)
{
  QString mp3_filename;
  Index index(location);
  if (!index.set_title_author_remix(to+".idx")) 
    // We add .idx otherwise the set_title_author_remix function will not find
    // a proper extension
    Debug("The new filename %s could not be parsed properly",
	  to.toAscii().data());
  mp3_filename=index.get_filename();
  index.write_idx();
  Song * song = database -> find(mp3_filename);
  if(!song)
    Debug("The filename %s is not visible in the songlist",
	  mp3_filename.toAscii().data());
  selector -> reread_and_repaint(song);
  return true;
};

//---------------------------------------------------------
//  Starting the stuff from within the song selector
//---------------------------------------------------------
void SongSelectorLogic::startRenamerOnSongs()
{
  QString text = QFileDialog::getExistingDirectory(this,
   "Specify directory to look for songs with wrong name");
  if (!text.isEmpty())
    {
      if (text.right(1)=="/")
	text = text.left(text.length()-1);
      UpdateSongFilename *usf=new UpdateSongFilename();
      RenamerLogic *renamer = new RenamerLogic(this,usf);
      renamer->scan(text);
      renamer->start_dialog();
    }
}

void SongSelectorLogic::startRenamerOnIndices()
{
  UpdateIndexedSong *updateIndexedSong = new UpdateIndexedSong(this);
  RenamerLogic *renamer = new RenamerLogic(this,updateIndexedSong);
  selectedSongIterator svi;
ITERATE_OVER(svi)

    renamer->add(svi.val());
  }
  renamer->start_dialog();
}
#endif // __loaded__renamer_cpp__
