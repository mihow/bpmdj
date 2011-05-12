/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__tags_cpp__
#define __loaded__tags_cpp__
using namespace std;
#line 1 "tags.c++"
#include <assert.h>
#include "constants.h"
#include "tags.h"
#include "memory.h"
#include "qstring-factory.h"

map<QString,tag_type> Tags::tree;
vector<QString>  Tags::tag_names;
bool             Tags::new_tags;

void Tags::init()
{
  // WVB -- tree.init();
  tag_names.clear();
  new_tags=false;
  int t = add_tag(EMPTY);
  assert(t==tag_end);
};

QString Tags::find_tag(tag_type idx)
{
  if (idx >=tag_names.size())
    {
      printf("Didnt find tag id %d\n",idx);
      fflush(stdout);
      assert(idx<tag_names.size());
    }
  return tag_names[idx];
}

tag_type Tags::add_tag(QString tag)
{
  if (tree.find(tag)==tree.end())
    {
      new_tags=true;
      tag_names.push_back(tag);
      int nr = tag_names.size()-1;
      assert(nr<256);
      tree[tag]=nr;
      return nr;
    }
  return tree[tag];
}

tag_type Tags::find_tag_create_if_necessary(QString tag)
{
  return add_tag(tag);
}

QString Tags::full_string(tags_type tags)
{
  if (tags == NULL) return EMPTY;
  int i=0;
  QString result("");
  while(tags[i]!=tag_end)
    result+=find_tag(tags[i++])+" ";
  return result;
}

tags_type Tags::parse_tags(QString tagstring)
{
  tags_type result = NULL;
  // first we should somehow count the number of tags.
  // Afterward we can insert them one by one
  int count = 0;
  char * original = strdup(tagstring.toAscii());
  char * runner=original;
  if (runner) 
    while(*runner)
      {
	while(*runner && *runner!=' ') runner++;
	count++;
	if (*runner) ++runner;
      }
  
  // allocate and fill array
  result = bpmdj_allocate(count+1,tag_type);
  char* tagss = original;
  runner = tagss;
  count = 0;
  if (runner)
    {
      while(*runner)
	{
	  while(*runner && *runner!=' ') runner++;
	  if (*runner)
	    {
	      *runner=0;
	      result[count++]=add_tag(tagss);
	      *runner=' ';
	      tagss=++runner;
	    }
	  else
	    result[count++]=add_tag(tagss);
	}
      bpmdj_deallocate(original);
    }
  result[count]=tag_end;
  return result;
}
#endif // __loaded__tags_cpp__
