/****
 Active Objects
 Copyright (C) 2006-2010 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__ao.html

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
#ifndef __loaded__ao_tracker_cpp__
#define __loaded__ao_tracker_cpp__
using namespace std;
#line 1 "ao-tracker.c++"
#include "ao-tracker.h"

elementResult ActiveAoTracker::sunset(string s)
{
#ifdef SUNSET_CHECK
  cerr << "The sun is setting for " << s << "\n";
#endif
  if (alive.find(s)==alive.end())
    {
      cerr << "sunset for unknown active object in " << (long)(void*)this << '\n';
      print();
      assert(0);
    }
  alive.erase(s);
  if (alive.empty())
    {
#ifdef SUNSET_CHECK
      cerr << "No active objects left, deactivating pool\n";
#endif
      deactivate();
    }
  return Done;
}

elementResult ActiveAoTracker::sunrise(string s)
{
#ifdef SUNSET_CHECK
  cerr << "The sun is rising for " << s << " in " << (long)(void*)this << "\n";
#endif
  if (alive.find(s)!=alive.end())
    {
      cerr << "Allocating second active object with name " << s 
	   << " in Active Object Tracker\n";
      exit(0);
    }
  alive.insert(s);
  return Done;
}

elementResult ActiveAoTracker::print()
{
  // prints the objects that are still active
  set<string>::iterator sit=alive.begin();
  cerr << "Active Objects still alive:\n";
  while(sit!=alive.end())
    {
      cerr << *sit << "\n";
      sit++;
    }
  return Done;
}
#endif // __loaded__ao_tracker_cpp__
