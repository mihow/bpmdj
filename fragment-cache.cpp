/****
 BpmDj v4.2-pl2: Free Dj Tools
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
#ifndef __loaded__fragment_cache_cpp__
#define __loaded__fragment_cache_cpp__
using namespace std;
#include <iostream>
#include <map>
#include <vector>
#include "map-iterator.h"
#include "set-iterator.h"
#include "bpmdj.h"
#include "do-fragment.h"
#include "fragment-creator.h"
#include "vector-iterator.h"
#include "config.h"
#include "selector.h"

void FragmentCreated::run()
{
  assert(selector);
  selector->fragmentCreated(this);
}

void FragmentCache::get(Song* song)
{
  deque<FragmentFile> ready = fragmentCreator->getReadyOnes();
  while (!ready.empty())
    {
      FragmentFile ff = ready.front();
      ready.pop_front();
      song2fragment[ff.get_song()]=ff;
    }
  FragmentFile ff = song2fragment[song];
  if (ff.isEmpty())
    fragmentCreator->createOneFor(song);
  else
    if (app) 
      app->postEvent(selector,new FragmentCreated(ff));
}
#endif // __loaded__fragment_cache_cpp__
