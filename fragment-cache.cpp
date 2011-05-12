/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
using namespace std;
#line 1 "fragment-cache.c++"
#include <iostream>
#include <map>
#include <vector>
#include "map-iterator.h"
#include "set-iterator.h"
#include "kbpm-dj.h"
#include "fragment.h"
#include "fragment-creator.h"
#include "vector-iterator.h"
#include "active-objects.h"
#include "config.h"
#include "songselector.logic.h"

void FragmentCreated::run(SongSelectorLogic * song_selector_window)
{
  if (song_selector_window)
    song_selector_window->fragmentCreated(this);
}

Fragment FragmentCreated::getFragment()
{
  if(f.isEmpty())
    return Fragment();
  return newFragment(f);
}

extern FragmentCreator fragmentCreator;
FragmentCache fragmentCache;

void FragmentCache::get(Song* song)
{
  deque<FragmentCreated> ready = fragmentCreator.getReadyOnes();
  while (!ready.empty())
    {
      FragmentCreated fc = ready.front();
      ready.pop_front();
      song2fragment[fc.song]=fc;
    }
  FragmentCreated fc = song2fragment[song];
  if (fc.isEmpty())
    fragmentCreator.createOneFor(song);
  else
    if (app) 
      app->postEvent(song_selector_window,
		     new FragmentCreated(fc));
}
