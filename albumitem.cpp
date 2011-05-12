/****
 BpmDj v3.8: Free Dj Tools
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
#ifndef __loaded__albumitem_cpp__
#define __loaded__albumitem_cpp__
using namespace std;
#line 1 "albumitem.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "albumitem.h"
//#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "dirscanner.h"

AlbumItem::AlbumItem(const char* name, Q3ListView* parent) :
  Q3ListViewItem(parent,name)
{
  init_song();
}

AlbumItem::AlbumItem(int nr, Song * s, Q3ListViewItem* parent) :
  Q3ListViewItem(parent,QString::number(nr),s->get_title(),s->get_author(),s->get_time(),s->get_storedin(),s->get_file())
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
