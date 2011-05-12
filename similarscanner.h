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
#ifndef __BPMDJ___SIMILARSCANNER_H__
#define __BPMDJ___SIMILARSCANNER_H__
using namespace std;
#line 1 "similarscanner.h++"
#include "dirscanner.h"
#include "similars.h"
#include "songselector.logic.h"

class SimilarScanner:
        public Similars,
	public DirectoryScanner
{
    Q_OBJECT
  private:
    SongSelectorLogic * selector;
    bool similarnames;
    void findSimilarNames(QString text, QString fullname);
    void similarNameFound(QString name, QString similar, QString fullname, QString fullsimilar, int dist);
    void getRidOff(QString fullname);
    virtual bool matchextension(const QString filename);
    void goFetch(QString name);
  protected:
    virtual void checkfile(const QString pathname, const QString filename);
  public:
    SimilarScanner(SongSelectorLogic* sroot);
    virtual void scan(const QString dirname);
    void setRoot(QString w);
  public slots:
    virtual void alreadyHave();
    virtual void ignore();
    virtual void goFetch();
    virtual void startStop();
};
#endif
