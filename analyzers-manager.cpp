/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__analyzers_manager_cpp__
#define __loaded__analyzers_manager_cpp__
using namespace std;
#line 1 "analyzers-manager.c++"
#include <qstring.h>
#include "analyzers-manager.h"
#include "selector.h"

void AnalyzersManager::start(SongSlot* slot, Song * song, 
			     QString command,
			     QString description)
{
  SongProcess* p=new SongProcess(slot,this);
  p->start(command.toAscii().data(),description.toAscii().data(),true);
  slot->start(song,p);
}

void AnalyzersManager::process_died(SongSlot* slot)
{
  Song * song = slot->getSong();
  slot->stop();
  selector->startAnotherAnalyzer(song,slot->getId());
}
#endif // __loaded__analyzers_manager_cpp__
