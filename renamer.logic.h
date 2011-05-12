/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__renamer_logic_h__
#define __loaded__renamer_logic_h__
using namespace std;
#line 1 "renamer.logic.h++"
#include <qlistview.h>
#include <qwidget.h>
#include "dirscanner.h"
#include "renamer.h"

class RenamerChangesFilename
{
 public:
  RenamerChangesFilename() {};
  virtual ~RenamerChangesFilename() {};
  virtual bool shouldFilenameBeExcluded(QString name) = 0;
  virtual void filenameChanged(QString from, QString to) = 0;
};

class RenamerLogic: 
  public Renamer, 
  public DirectoryScanner
{
   Q_OBJECT
 public:
   RenamerLogic(QWidget* parent, RenamerChangesFilename * rcf);
   virtual ~RenamerLogic();
   void add(const QString name, const QString pos);
   void scan(const QString dirname);
 protected:
   virtual void checkfile(const QString fullname, const QString filename);
   virtual bool matchextension(const QString filename);
 private:
   RenamerChangesFilename * inform;
   QString smallCapsInWord(QString in);
   QString capitalizeAfterSpace(QString in);
   QString removeSpaces(QString in);
   QString removeSpecials(QString in);
   QString removeFirstSpecials(QString in);
   QString removeFirstChar(QString in);
   QString subStringIsAuthor(QString in);
   QString replaceSubString(QString in);
   QString replaceUnderscores(QString in);
   QString beforeMinusIsAuthor(QString in);
   QString fixExtention(QString in);
   QString betweenBracesIsTrash(QString in);
   QString betweenBracesIsMix(QString in);
   QString subStringIsMix(QString in);
   QString deleteSubString(QString in);
 public slots:
   virtual void capitalizeAfterSpace();
   virtual void smallCapsInWords();
   virtual void removeSpaces();
   virtual void removeSpecials();
   virtual void changeSelection();
   virtual void deleteSubString();
   virtual void subStringIsAuthor();
   virtual void removeFirstChar();
   virtual void replaceSubString();
   virtual void realizeSelection();
   virtual void ignoreSelection();
   virtual void beforeMinusIsAuthor();
   virtual void betweenBracesIsTrash();
   virtual void betweenBracesIsMix();
   virtual void substringIsMix();
   virtual void removeFirstSpecials();
   virtual void keySelectionIsAuthor();
   virtual void replaceUnderscores();
   virtual void fixExtention();
};
#endif // __loaded__renamer_logic_h__
