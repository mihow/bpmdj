/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__set_iterator_h__
#define __loaded__set_iterator_h__
using namespace std;
#line 1 "set-iterator.h++"
#include <set>
#include <assert.h>
#include "iterator.h"

template<class K>
class simpleSetIterator
{
  typename set<K>::iterator it,end;
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
  simpleSetIterator(set<K> &m): it(m.begin()), end(m.end()), once(false)
  {
  };
  simpleSetIterator(set<K> *m): it(m->begin()), end(m->end()), once(false)
  {
  };
};


template<class K, class C>
class setIterator
{
  typename set<K,C>::iterator it,end;
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
  setIterator(set<K,C> &m): it(m.begin()), end(m.end()), once(false)
  {
  };
  setIterator(set<K,C> *m): it(m->begin()), end(m->end()), once(false)
  {
  };
};

template<class K, class C>
class multisetIterator
{
  typename multiset<K,C>::iterator it,end;
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
  multisetIterator(multiset<K,C> &m): it(m.begin()), end(m.end()), once(false)
  {
  };
  multisetIterator(multiset<K,C> *m): it(m->begin()), end(m->end()), once(false)
  {
  };
};
#endif // __loaded__set_iterator_h__
