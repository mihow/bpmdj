/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
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
#include "common.h"
#include "avltree.h"
#include "growing-array.h"
#include "tag-type.h"

class Tag2Index: public Node<QString>
{
 public:
  QString  tag;
  int      index;
  virtual QString getKey() { return tag ; };
  virtual int compareAddData(Node<QString>* other);
  virtual int compareSearchData(QString key);
  virtual void print() {};
  Tag2Index(QString t, int nr);
  virtual ~Tag2Index() {};
};

class Tags
{
  private:
    static AvlTree      <QString> tree;
    static QString   find_tag(tag_type tag);
    static tag_type  add_tag(QString tag);
  public:
    static GrowingArray <QString> tag_names;
    static bool      new_tags;
    static void      init();
    static tag_type  find_tag(QString tag);
    static tags_type parse_tags(QString tag_string);
    static QString   full_string(tags_type tags);
};
