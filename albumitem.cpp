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

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <qlistview.h>
#include "songselector.logic.h"
#include "qsong.h"
#include "process-manager.h"
#include "dirscanner.h"
#include "spectrum.h"

AlbumItem::AlbumItem(const char* name, QListView* parent) :
  QListViewItem(parent,name)
{
  song = NULL;
}

AlbumItem::AlbumItem(int nr, Song * s, QListViewItem* parent) :
  QListViewItem(parent,QString::number(nr),s->get_title(),s->get_author(),s->get_time(),s->get_storedin(),s->get_file())
{
  song = s;
  setRenameEnabled(0,true);
  fixNr();
}

void AlbumItem::fixNr()
{
  if (text(0).length()<2)
    setText(0,"0"+text(0));
}
