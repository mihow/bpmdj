/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__renamer_h__
#define __loaded__renamer_h__
using namespace std;
#line 1 "renamer.h++"
#include <qlistview.h>
#include <qwidget.h>
#include "dirscanner.h"
#include "ui-renamer.h"
#include "song.h"

class RenamerChangesFilename
{
public:
  RenamerChangesFilename() 
  {
  };
  virtual ~RenamerChangesFilename() 
  {
  };
  /**
   * Change should return false if an error occured
   */
  virtual bool change(QString from, QString to, QString location) =0;
};

class RenamerLogic: 
  public QDialog,
  public Ui::Renamer, 
  public DirectoryScanner
{
   Q_OBJECT
 public:
   RenamerLogic(QWidget* parent, RenamerChangesFilename * rcf);
   virtual ~RenamerLogic();
   void add(const QString name, const QString pos);
   void add(Song* song);
   void start_dialog();
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
#endif // __loaded__renamer_h__
