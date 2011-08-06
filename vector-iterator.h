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
#ifndef __loaded__vector_iterator_h__
#define __loaded__vector_iterator_h__
using namespace std;
#include <vector>
#include <assert.h>
#include "iterator.h"

template<class K>
class vectorIterator
{
  typename vector<K>::iterator it,end;
  bool once;
public:
  inline K key()
  {
    assert(0);
  }
  inline K val()
  {
    return *it;
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
    if (once) return false;
    return once=true;
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  vectorIterator(vector<K> &m): it(m.begin()), end(m.end()), once(false)
  {
  };
  vectorIterator(vector<K> *m): it(m->begin()), end(m->end()), once(false)
  {
  };
};

template<class K>
class constVectorIterator
{
  typename vector<K>::const_iterator it,end;
  bool once;
public:
  inline K key()
  {
    assert(0);
  }
  inline K val()
  {
    return *it;
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
    if (once) return false;
    return once=true;
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  constVectorIterator(const vector<K> &m): it(m.begin()), end(m.end()), 
					   once(false)
  {
  };
  constVectorIterator(const vector<K> *m): it(m->begin()), end(m->end()), 
					   once(false)
  {
  };
};

#endif // __loaded__vector_iterator_h__
