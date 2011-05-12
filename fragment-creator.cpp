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
  QString fragment_filename = FragmentsDir+"/nr"+QString::number((intptr_t)(void*)song)+".wav";
  Index index(song->get_storedin());
  unsigned8 pos = index.get_cue();
  int startsec=pos/44100;
  startsec--;
  if (startsec<=0) // we start @ 25 % of the song if no cues
    startsec = index.get_time_in_seconds()/4;
  int stopsec = startsec+10; // always 10 seconds playing time
  
  if (!exists(fragment_filename.ascii()))
    {
      // this is a bit experimental and relies hevaily onmplayer its
      // excellent capabilities to seek to the right position.
      QString toexecute = 
	QString("mplayer -vc null -vo null -ss ")
	+ QString::number(startsec)
	+ QString(" -endpos ")
	+ QString::number(stopsec-startsec)
	+ QString(" -ao pcm:fast:file=")
	+ QString(escape(fragment_filename))
	+ OneSpace
	+ QString(MusicDir)
	+ slash
	+ QString(escape(music_filename));
      execute(toexecute.ascii());
    }
  if (exists(fragment_filename.ascii()))
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
	app->postEvent(song_selector_window,
		       new FragmentCreated(ff));
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
  deactivate();
  return Done;
}

FragmentCreator fragmentCreator;
#endif // __loaded__fragment_creator_cpp__
