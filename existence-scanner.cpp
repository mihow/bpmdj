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

#include "existence-scanner.h"
#include "songselector.logic.h"
#include "kbpm-dj.h"

void ExistenceScanner::run()
{
  int i = 0;
  while(i < all->count)
    {
      Song * song = all->elements[i];
      song -> checkondisk();
      i++;
    }
}

void ExistenceScanner::stoppedAnalyzing()
{
  // the receiver of this event will delete this object.
  // so we cannot do anything after sending this
  app->postEvent(song_selector_window, new ExistenceScannerFinished(this));
}

ExistenceScanner::~ExistenceScanner()
{
  delete all;
}

void ExistenceScannerFinished::run(SongSelectorLogic * song_selector_window)
{
  status->message("Finished checking availability of songs",10000);
  song_selector_window->updateItemList();
  delete thread;
}
