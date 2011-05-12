/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef _DIRSCANNER_H
#define _DIRSCANNER_H

#include <stdlib.h>
#include <qstring.h>

// the checkname below is the prefix that is placed before directories when passed back to the caller
class DirectoryScanner
{
 protected:
    QString required_extension;
    virtual void recursing(const QString  dirname);
    virtual void scanfile(const QString  fullname, const QString  filename);
    virtual void checkfile(const QString  fullname, const QString  filename) = 0;
    virtual void scandir(const QString  dirname, const QString  checkname);
    virtual bool matchextension(const QString  filename);
    virtual ~DirectoryScanner() {};
 public:
    DirectoryScanner(QString  extension);
    static bool goodName(QString name);
    static bool songExtension(QString extension);
    static bool goodExtension(QString extension);
    virtual void scan(const QString  dirname, const QString  checkname)
      { scandir(dirname, checkname); };
};
#endif
