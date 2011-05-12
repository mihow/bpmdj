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

#ifndef __loaded__reference_count_h__
#define __loaded__reference_count_h__
using namespace std;
#line 1 "reference-count.h++"
#include <assert.h>
#include "cmpxchg.h"

/**
 * @brief counts the number of smartpointers refering to it
 * 
 * Keeps track of the number of reference floating around to this object.
 * This class is resistent to concurrent access. In other words the 
 * reference_count will be atomically increased and decreased
 */
class ReferenceCount
{
 public:
  /**
   * The number of smart pointers refering to this object.
   * 0 if none.
   */
  int reference_count;
  bool deleting;
  ReferenceCount(): reference_count(0), deleting(false)
  {
  }
  ReferenceCount(const ReferenceCount&): reference_count(0), deleting(false)
  {
  }
  ReferenceCount& operator =(const ReferenceCount&)
  {
    reference_count=0;
    return *this;
  }
  /**
   * atomically increases the reference count.
   */
  void incref() 
  {
    int before;
    do 
      {
	before = reference_count;
      }
    while(cmpxchg(&reference_count,before,before+1)!=before);
  }
  /**
   * decreases the refernce count. Returns the new reference count.
   */
  int decref()
  {
    int before;
    do 
      {
	before = reference_count;
      }
    while(cmpxchg(&reference_count,before,before-1)!=before);
    assert(before-1>=0);
    return before-1;
  }
  /**
   * destroys the object. In this case performs a sanity check 
   * on the refernec_count.
   */
  virtual ~ReferenceCount()
  {
    assert(reference_count==0);
  }
};

/**
 * @brief Smart pointer to reference counted objects.
 * @todo actually check whether we cannot create a concurrency problem.
 */
template <class ReferenceCountedObject> 
class Smart
{
public:
  ReferenceCountedObject* ptr;
  void incref() const
    {
      if (ptr)
	ptr->incref();
    }
  void decref() const
    {
      if (ptr && ptr->decref()==0 && !ptr->deleting)
	{
	  ptr->deleting=true;
	  delete ptr;
	}
    }
 public:
  explicit Smart(ReferenceCountedObject* p = 0) : ptr(p)
  {
    incref();
  }
  Smart(const Smart<ReferenceCountedObject>& o) : ptr(o.ptr)
  {
    incref();
  }
  Smart<ReferenceCountedObject>& operator=(const Smart<ReferenceCountedObject>&o)
  {
    o.incref();
    decref();
    ptr=o.ptr;
    return *this;
  }
  template <class AnotherReferenceCountedObject>
  Smart<ReferenceCountedObject>& operator=(const Smart<AnotherReferenceCountedObject>&o)
  {
    o.incref();
    decref();
    ptr=(ReferenceCountedObject*)o.ptr;
    return *this;
  }

  bool& operator==(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr==o.ptr;
    }
  bool& operator!=(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr!=o.ptr;
    }
  bool& operator<(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr<o.ptr;
    }
  bool& operator<=(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr<=o.ptr;
    }
  bool& operator>=(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr>=o.ptr;
    }
  ~Smart()
    {
      decref();
    }
  ReferenceCountedObject& operator*()
    {
      return *ptr;
    }
  ReferenceCountedObject* operator->()
    {
      return ptr;
    }
  operator bool()
    {
      return ptr;
    }
  template <class AnotherReferenceCountedObject> 
  operator Smart<AnotherReferenceCountedObject>()
  {
    return Smart<AnotherReferenceCountedObject>((AnotherReferenceCountedObject*)ptr);
  }
};
#endif // __loaded__reference_count_h__
