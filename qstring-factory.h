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

#include <qstring.h>
#include "avltree.h"

extern const QString TRUE_TEXT;
extern const QString FALSE_TEXT;
extern const QString TAG_TRUE; 
extern const QString TAG_FALSE;
extern const QString zero;
extern const QString zeroo;
extern const QString slash;
extern const QString EMPTY;

QString tonumber(const int b);
QString tonumber(const float f);

class QStringFactory
{
  private:
    static bool killed;
    static AvlTree<QString> tree;
    static QString * lookup(QString *str);
  public:
    // WVB -- try to majke this const char *str)
    static QString create(char* str);
    static void kill();
};
