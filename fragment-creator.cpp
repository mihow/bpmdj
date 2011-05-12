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
#line 1 "fragment-creator.c++"
#include "kbpm-dj.h"
#include "scripts.h"
#include "songselector.logic.h"
#include "fragment-creator.h"

elementResult ActiveFragmentCreator::createOneFor(Song* song)
{
  QString music_filename = song->get_file();
  QString fragment_filename = FragmentsDir+"/nr"+QString::number((int)(void*)song)+".wav";
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
      QString toexecute = "mplayer -vc null -vo null -ss "+QString::number(startsec)
	+ " -endpos "+QString::number(stopsec-startsec)+" -ao pcm:fast:file="+escape(fragment_filename)
	+ " "+MusicDir+"/"+escape(music_filename);
      execute(toexecute.ascii());
    }
  if (exists(fragment_filename.ascii()))
    {
      FragmentCreated fc(song, fragment_filename);
      {
	Synchronized(this);
	ready.push_back(fc);
      }
      if (app)
	app->postEvent(song_selector_window,
		       new FragmentCreated(fc));
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

deque<FragmentCreated> FragmentCreator::getReadyOnes()
{
  Synchronized(object);
  deque<FragmentCreated> result = object.ready;
  object.ready.clear();
  return result;
}

elementResult ActiveFragmentCreator::terminate()
{
  deactivate();
  return Done;
}

FragmentCreator fragmentCreator;
