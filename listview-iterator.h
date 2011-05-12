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
#ifndef __BPMDJ___LISTVIEW_ITERATOR_H__
#define __BPMDJ___LISTVIEW_ITERATOR_H__
using namespace std;
#line 1 "listview-iterator.h++"
#include <vector>
#include <assert.h>
#include "qsong.h"
#include "iterator.h"

template<class T>
class listViewIterator
{
  QListViewItemIterator it;
  bool once;
public:
  inline void key()
  {
    assert(0);
  }
  inline T* val()
  {
    return (T*)it.current();
  }
  inline bool valid()
  {
    return it.current();
  }
  inline void step()
  {
    ++it;
  }
  inline bool prepare_block()
  {
    if (once) return false;
    once=true;
    return valid();
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  listViewIterator(QListViewItem* C): it(C), once(false)
  {
  };
  listViewIterator(QListView* C): it(C), once(false)
  {
  };
};
#endif
