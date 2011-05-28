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

const unsigned4 TAGS_TEXT=0;
const unsigned4 TAGS_OR=1;
const unsigned4 TAGS_AND=2;
const unsigned4 TAGS_NOT=3;
#endif // __loaded__tags_h__
