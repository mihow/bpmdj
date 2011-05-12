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

#include <qlistview.h>
#include <qwidget.h>
#include "dirscanner.h"
#include "renamer.h"

class RenamerLogic: 
  public Renamer, 
  public DirectoryScanner
{
   Q_OBJECT
 public:
   RenamerLogic(QWidget* parent=0, const QString name=0);
   void add(const QString name, const QString pos);
 protected:
   virtual void checkfile(const QString fullname, const QString filename);
   virtual bool matchextension(const QString filename);
 private:
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
   virtual void substringIsMix();
   virtual void removeFirstSpecials();
   virtual void keySelectionIsAuthor();
   virtual void replaceUnderscores();
   virtual void fixExtention();
   virtual void betweenBracesIsMix();
};
