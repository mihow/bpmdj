/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__analyzers_manager_h__
#define __loaded__analyzers_manager_h__
using namespace std;
#line 1 "analyzers-manager.h++"
#include "song-process.h"
class Song;
class SongSlot;
class SongSelectorLogic;

class AnalyzersManager: public DeathProcessListener
{
private:
  virtual void process_died(SongSlot*);
public:
  virtual void start(SongSlot*, Song* song, 
		     QString command, 
		     QString description);
};
#endif // __loaded__analyzers_manager_h__
