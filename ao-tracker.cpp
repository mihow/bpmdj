/****
 Borg IV
 Copyright (C) 2006-2007 Werner Van Belle

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
  // cerr << "The sun is setting for " << s << "\n";
  assert(alive.find(s)!=alive.end());
  alive.erase(s);
  if (alive.empty())
    {
      // cerr << "No active objects left, deactivating pool\n";
      deactivate();
    }
  return Done;
}

elementResult ActiveAoTracker::sunrise(string s)
{
  // cerr << "The sun is rising for " << s << "\n";
  if (alive.find(s)!=alive.end())
    {
      cerr << "Allocating second active object with name " << s 
	   << " in Active Object Tracker\n";
      exit(0);
    }
  alive.insert(s);
  return Done;
}
#endif // __loaded__ao_tracker_cpp__
