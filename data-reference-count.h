/****
 Borg4 Data Library
 Copyright (C) 2005-2009 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__data.html

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
#ifndef __loaded__data_reference_count_h__
#define __loaded__data_reference_count_h__
using namespace std;
#line 1 "data-reference-count.h++"
#include <set>
#include <stdio.h>
#include <typeinfo>
#include "reference-count.h"

/**
 * @brief counts the number of smartpointers refering to it and also keeps a 
 * readable/writeable flag.
 */
#ifdef DATA_RO_FIELD
typedef enum {ReadOnly, Writeable} DataAccessFlag;
#define W2R_CACHING
#define CLONE_STATS

template<class T> 
class DataKeyCmp
{
public:
  bool operator()(T* a, T* b) 
  {
    return a->smaller_than(b);
  };
};
#endif

class DataReferenceCount: public ReferenceCount
{
public:
#ifdef DATA_RO_FIELD
  bool readonly;
#endif
  /**
   * if the object is being cloned to a readonly copy
   * then this variable will point to the readonly clone. This 
   * variable is cleared as soobn as we write anything to this object;
   */
  Smart<ReferenceCount> readonly_clone;
  DataReferenceCount()
#ifdef DATA_RO_FIELD
    : readonly(false)
#endif
  {
  }

  /**
   * Should instantiate a copy of this own object through 
   * using the copy constructor.
   */
  virtual DataReferenceCount* clone() = 0;
  /**
   * should make all the values in this object readonly.
   */
#ifdef DATA_RO_FIELD
  virtual void change_access(DataAccessFlag flag)
  {
    if (flag==ReadOnly)
      readonly=true;
    else if (flag==Writeable)
      readonly=false;
    else assert(0);
  }
  void will_write()
  {
    readonly_clone=Smart<ReferenceCount>();
    assert(!readonly);
  }
#endif
  /**
   * Dont assign. Bad idea :)
   */
  DataReferenceCount& operator =(const DataReferenceCount& r)
  {
    assert(0);
  }
  /**
   * This function is called to reallocate the specific object
   * The return value must be the new pointer it will become
   * If this is a truly new object we return the same one, otherwise we
   * need to return the old one. 
   */
  virtual DataReferenceCount* reallocate()
  {
    assert(0);
  };
};

#ifdef DATA_RO_FIELD
#ifdef CLONE_STATS
extern long fast_comparisons;
extern long slow_comparisons;
extern long r2w_cloned;
extern long w2r_cached;
extern long w2r_real_clone;
extern long w2r_known_clone;
extern long w2r_known_singleref;
extern long w2r_unknown_singleref;
#endif
#endif

/**
 * This class uses an ugly trick in which we misalign the last bit of the pointer
 * as an indication of the fact that we should clone the pointer to a writeable
 * one. As soon as this poitner is copied, we check whether we really need to clone 
 * the original or not
 */
template <class ReferenceCountedObject> 
class DataSmart: public Smart<ReferenceCountedObject>
{
public:
  explicit DataSmart(ReferenceCountedObject* p = 0) : Smart<ReferenceCountedObject>(p)
  {
  }
  /*
  DataSmart(const DataSmart<ReferenceCountedObject>& o) 
  {
    ((DataSmart<ReferenceCountedObject>*)(void*)(&o))->make_truly_writeable();
    should_become_writeable=false;
    Smart<ReferenceCountedObject>::ptr=o.ptr;
    Smart<ReferenceCountedObject>::incref();
  }

  DataSmart<ReferenceCountedObject>& operator=(const DataSmart<ReferenceCountedObject>&o)
  {
    ((DataSmart<ReferenceCountedObject>*)(void*)(&o))->make_truly_writeable();
    should_become_writeable=false;
    Smart<ReferenceCountedObject>::operator =(o);
    return *this;
  }
  
  template <class AnotherReferenceCountedObject>
  DataSmart<ReferenceCountedObject>& operator=(const DataSmart<AnotherReferenceCountedObject>&o)
  {
    ((DataSmart<ReferenceCountedObject>*)(void*)(&o))->make_truly_writeable();
    should_become_writeable=false;
    Smart<ReferenceCountedObject>::operator =(o);
    return *this;
  }
  */

#ifdef DATA_RO_FIELD
  void change_access(DataAccessFlag flag)
  {
    if (flag==ReadOnly) make_readonly();
    else if(flag==Writeable) make_writeable();
    else assert(0);
  }
#endif
  
  bool operator<(const DataSmart<ReferenceCountedObject> & other) const
  {
    if (this->ptr==NULL) return other.ptr;
    if (other.ptr==NULL) return false;
#ifdef DATA_RO_FIELD
    if (this->ptr->readonly && other.ptr->readonly)
      {
	fast_comparisons++;
	return this->ptr<other.ptr;
      }
    slow_comparisons++;
#endif
    if (this->ptr==other.ptr) return false;
    if (typeid(*(this->ptr))==typeid(*other.ptr))
      return this->ptr->most_specialized_smaller_than(other.ptr);
    return typeid(*(this->ptr)).before(typeid(*(other.ptr)));
  }
  
#ifdef DATA_RO_FIELD
  /**
   * Makes the content of this pointer readonly by copying it if necessary
   */
  void make_readonly()
  {
    if (this->ptr==NULL) return;
    if (this->ptr->readonly) return;
    /** 
     * if we have aprevious readonly clone of this object then we create 
     * a new pointer to it
     */
#ifdef W2R_CACHING
    if (this->ptr->readonly_clone.ptr!=NULL)
      {
#ifdef CLONE_STATS
	w2r_cached++;
#endif
	DataReferenceCount* cloned = (DataReferenceCount*)Smart<ReferenceCountedObject>::ptr->readonly_clone.ptr;
	cloned->incref();
	Smart<ReferenceCountedObject>::decref();
	Smart<ReferenceCountedObject>::ptr=(ReferenceCountedObject*)cloned;
      }
    else
#endif
      /**
       * If we have the only reference then we don't care about the ... well 'other' reference
       */
      if (this->ptr->reference_count==1)
	{
	  this->ptr->change_access(ReadOnly);
	  assert(this->ptr->readonly);
	  DataReferenceCount* unique = this->ptr->reallocate();
	  if (unique==this->ptr)
	    {
#ifdef CLONE_STATS
	      w2r_unknown_singleref++;
#endif
	    }
	  else
	    {
#ifdef CLONE_STATS
	      w2r_known_singleref++;
#endif
	      assert(unique->readonly);
	      unique->incref();
	      Smart<ReferenceCountedObject>::decref();
	      this->ptr=(ReferenceCountedObject*)unique;
	    }
	}
      else
	{
	  DataReferenceCount* unique;
	  {
	    DataReferenceCount* cloned = Smart<ReferenceCountedObject>::ptr->clone();
	    cloned->readonly=false;
	    cloned->change_access(ReadOnly);
	    assert(cloned->readonly);
	    unique = cloned->reallocate();
	    if (unique==cloned)
	      {
#ifdef CLONE_STATS
		w2r_real_clone++;
#endif
	      }
	    else
	      {
		delete(cloned);
#ifdef CLONE_STATS
		w2r_known_clone++;
#endif
	      }
	  }
	  assert(unique->readonly);
	  unique->incref();
	  this->ptr->readonly_clone=Smart<ReferenceCount>(unique);
	  Smart<ReferenceCountedObject>::decref();
	Smart<ReferenceCountedObject>::ptr=(ReferenceCountedObject*)unique;
	assert(Smart<ReferenceCountedObject>::ptr->readonly);
	}
  }
  void make_writeable()
  {
    if (!Smart<ReferenceCountedObject>::ptr) return;
    if (!Smart<ReferenceCountedObject>::ptr->readonly) return;
#ifdef CLONE_STATS
    r2w_cloned++;
#endif
    DataReferenceCount* cloned = Smart<ReferenceCountedObject>::ptr->clone();
    cloned->readonly_clone=Smart<ReferenceCount>(this->ptr);
    cloned->readonly=false;
    cloned->incref();
    Smart<ReferenceCountedObject>::decref();
    Smart<ReferenceCountedObject>::ptr=(ReferenceCountedObject*)cloned;
    Smart<ReferenceCountedObject>::ptr->change_access(Writeable);
    assert(!Smart<ReferenceCountedObject>::ptr->readonly);
  }
#endif
};


#ifdef DATA_RO_FIELD
//#define PRINT_MAPPINGS
extern unsigned long long readonly_allocated;
extern unsigned long long readonly_not_allocated;
template<class T>
class Uniqueifier
{
  set<T*,DataKeyCmp<T> > c;
public:
  T* reallocate(T* a)
  {
    assert(a->readonly);
    a->readonly=false;
    if (c.find(a)==c.end()) 
      {
#ifdef CLONE_STATS
	readonly_allocated+=sizeof(T);
#endif
	a->incref();
	c.insert(a);
	a->readonly=true;
	return a;
      }
#ifdef CLONE_STATS
    readonly_not_allocated+=sizeof(T);
#endif
    T* r = *(c.find(a));
    assert(r!=a);
#ifdef PRINT_MAPPINGS
    printf("Mapping ");
    a->print(1);
    printf(" ==> ");
    r->print(1);
    printf("\n");
    if ((r->smaller_than(a) || a->smaller_than(r)) // the thing is actually equal
	|| r->smaller_than(r)) // the thing has an improper comparison operator
      {
	printf("ERROR\n");
	fflush(stdout);
	exit(0);
      }
#endif
    a->readonly=true;
    return r;
  }
};
#endif
#endif // __loaded__data_reference_count_h__
