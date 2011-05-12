/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#include "tags.h"
#include "memory.h"
#include "avltree.h"
#include "qstring-factory.h"
#include "growing-array.h"

Tag2Index::Tag2Index(QString pt, int idx) :
  Node<QString>()
{
  assert(!pt.isNull());
  index = idx;
  tag = pt;
};

int Tag2Index::compareAddData(Node<QString>* n)
{
  Tag2Index *other = (Tag2Index*)n;
  return compareSearchData(other->tag);
}

int Tag2Index::compareSearchData(QString b)
{
  QString a = tag;
  assert(!a.isNull());
  assert(!b.isNull());
  // if (a.isNull() && b.isNull()) return 0;
  // if (a.isNull()) return -1;
  // if (b.isNull()) return 1;
  if (a<b) return -1;
  if (a==b) return 0;
  if (a>b) return 1;
  return 1;
}

AvlTree<QString>      Tags::tree;
GrowingArray<QString> Tags::tag_names;
bool                  Tags::new_tags;

void Tags::init()
{
  tree.init();
  tag_names.init();
  new_tags=false;
  int t = add_tag(EMPTY);
  assert(t==tag_end);
};

QString Tags::find_tag(tag_type idx)
{
  if (idx >=tag_names.count)
    {
      printf("Didnt find tag id %d\n",idx);
      fflush(stdout);
      assert(idx<tag_names.count);
    }
  return tag_names.elements[idx];
}

tag_type Tags::find_tag(QString tag)
{
  Tag2Index * found;
  found = (Tag2Index*)tree.search(tag);
  assert(found);
  return found->index;
}

tag_type Tags::add_tag(QString tag)
{
  Tag2Index * found = (Tag2Index*)tree.search(tag);
  if (!found)
    {
      new_tags=true;
      int nr = tag_names.add(tag);
      assert(nr<256);
      found=new Tag2Index(tag,nr);
      tree.add(found);
      // printf("Adding tag :%s(%d)\n",(const char*)tag,found->index);
    }
  return found->index;
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
  char * original = strdup(tagstring);
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
