/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__albumitem_cpp__
#define __loaded__albumitem_cpp__
using namespace std;
#line 1 "albumitem.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "albumitem.h"
#include "qsong.h"
#include "dirscanner.h"

AlbumItem::AlbumItem(const char* name, Q3ListView* parent) :
  Q3ListViewItem(parent,name)
{
  init_song();
}

AlbumItem::AlbumItem(int nr, Song * s, Q3ListViewItem* parent) :
  Q3ListViewItem(parent,QString::number(nr),s->get_title(),s->get_author(),
		 s->get_time(),s->get_storedin(),s->get_file())
{
  set_song(s);
  setRenameEnabled(0,true);
  fixNr();
}

void AlbumItem::fixNr()
{
  if (text(0).length()<2)
    setText(0,QString("0")+text(0));
}
#endif // __loaded__albumitem_cpp__
