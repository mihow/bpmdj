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
#ifndef __loaded__fragment_creator_cpp__
#define __loaded__fragment_creator_cpp__
using namespace std;
#line 1 "fragment-creator.c++"
#include "bpmdj.h"
#include "scripts.h"
#include "selector.h"
#include "fragment-creator.h"

elementResult ActiveFragmentCreator::createOneFor(Song* song)
{
  QString music_filename = song->get_file();
  QString fragment_filename = FragmentsDir+"/nr"+
    QString::number((intptr_t)(void*)song)+".wav";
  Index index(song->get_storedin());
  unsigned8 pos = index.get_cue();
  int startsec=pos/metarate;
  startsec--;
  if (startsec<=0) // we start @ 25 % of the song if no cues
    startsec = index.get_time_in_seconds()/4;
  int stopsec = startsec+10; // always 10 seconds playing time
  
  if (!exists(fragment_filename))
    {
      /**
       * this relies heavily on mplayer its
       * excellent capabilities to seek to the right position.
       */
      QString description=QString("Creating fragment ")+song->getDisplayTitle();
      QString toexecute=QString("mplayer -vc null -vo null -ss ")
	+ QString::number(startsec)
	+ QString(" -endpos ")
	+ QString::number(stopsec-startsec)
	+ QString(" -ao pcm:fast:file=")
	+ escape(fragment_filename)
	+ OneSpace
	+ QString(MusicDir)
	+ slash
	+ escape(music_filename)
	+ QString(">/dev/null 2>/dev/null");
      execute(description,toexecute);
    }
  if (exists(fragment_filename))
    {
      /** 
       * to avoid that we create the same fragment multiple
       * times and thereby needlessly remove the file
       * we keep track of all the fragments we created already
       */
      FragmentFile ff;
      if (created.find(song)!=created.end())
	ff=created[song];
      else
	created[song]=ff=FragmentFile(song, fragment_filename);
      {
	Synchronized(this);
	ready.push_back(ff);
      }
      if (app)
	app->postEvent(selector, new FragmentCreated(ff));
    }
  return Done;
}
 
bool ActiveFragmentCreator::handle()
{
  if (handling.empty()) 
    return false;
  handle(handling.back());
  handling.pop_back();
  return !handling.empty();
}

deque<FragmentFile> FragmentCreator::getReadyOnes()
{
  Synchronized(object);
  deque<FragmentFile> result = object.ready;
  object.ready.clear();
  return result;
}

elementResult ActiveFragmentCreator::terminate()
{
  return deactivate();
}

FragmentCreator fragmentCreator;
#endif // __loaded__fragment_creator_cpp__
