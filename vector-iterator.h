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
#ifndef __BPMDJ___VECTOR_ITERATOR_H__
#define __BPMDJ___VECTOR_ITERATOR_H__
using namespace std;
#line 1 "vector-iterator.h++"
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
  constVectorIterator(const vector<K> &m): it(m.begin()), end(m.end()), once(false)
  {
  };
  constVectorIterator(const vector<K> *m): it(m->begin()), end(m->end()), once(false)
  {
  };
};

#endif
