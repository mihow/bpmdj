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
#ifndef __loaded__map_iterator_h__
#define __loaded__map_iterator_h__
using namespace std;
#line 1 "map-iterator.h++"
#include "iterator.h"

/**
 * A map iterator
 */
template<class K, class V>
class mapIterator
{
  typename map<K,V>::iterator it,end;
public:
  inline K key()
  {
    return it->first;
  }
  inline V& val()
  {
    return it->second;
  }
  inline bool valid()
  {
    return it!=end;
  }
  inline void step()
  {
    it++;
  }
  inline bool prepare_block()
  {
    return it!=end;
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  mapIterator(map<K,V> &m): it(m.begin()), end(m.end())
  {
  };
};
#endif // __loaded__map_iterator_h__
