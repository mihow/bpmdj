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
#ifndef __loaded__tags_h__
#define __loaded__tags_h__
using namespace std;
#line 1 "tags.h++"
#include <qstring.h>
#include <map>
#include <vector>
#include "common.h"
#include "tag-type.h"

class Tags
{
 private:
  static map<QString,tag_type> tree;
  static QString   find_tag(tag_type tag);
 public:
  static tag_type  add_tag(QString tag);
  static vector<QString> tag_names;
  static bool      new_tags;
  static void      init();
  static tag_type  find_tag(QString tag)
  {
    return tree[tag];
  }
  static tag_type  find_tag_create_if_necessary(QString tag);
  static tags_type parse_tags(QString tag_string);
  static QString   full_string(tags_type tags);
};
#endif // __loaded__tags_h__
