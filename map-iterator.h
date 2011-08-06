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
#ifndef __loaded__map_iterator_h__
#define __loaded__map_iterator_h__
using namespace std;
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
