#ifndef __loaded__reference_count_h__
#define __loaded__reference_count_h__
using namespace std;
#line 1 "reference-count.h++"
#include <assert.h>
#ifndef SINGLE_THREADED
#include "cmpxchg.h"
#endif

/**
 * @brief counts the number of smartpointers refering to it
 * 
 * Keeps track of the number of reference floating around to this object.
 * This class is resistent to concurrent access. In other words the 
 * reference_count will be atomically increased and decreased
 */

//#define SINGLE_THREADED
class ReferenceCount
{
 public:
  /**
   * The number of smart pointers refering to this object.
   * 0 if none.
   */
  volatile int reference_count;
  ReferenceCount(): reference_count(0)
  {
  }
  ReferenceCount(const ReferenceCount&): reference_count(0)
  {
  }
  ReferenceCount& operator =(const ReferenceCount& o)
  {
    assert(&o!=this);
    reference_count=0;
    return *this;
  }
  /**
   * atomically increases the reference count.
   */
  void incref() 
  {
#ifdef SINGLE_THREADED
    assert(reference_count>=0);
    reference_count++;
#else
    int before;
    do 
      {
	before = reference_count;
      }
    while(cmpxchg(&reference_count,before,before+1)!=before);
    assert(before>=0);
#endif
  }
  /**
   * decreases the refernce count. Returns the new reference count.
   */
  int decref()
  {
#ifdef SINGLE_THREADED
    return --reference_count;
#else
    int before;
    do 
      {
	before = reference_count;
      }
    while(cmpxchg(&reference_count,before,before-1)!=before);
    return before-1;
#endif
  }
  /**
   * destroys the object. In this case performs a sanity check 
   * on the refernce_count.
   */
  virtual ~ReferenceCount()
  {
    assert(reference_count==-1);
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
  /**
   * The general idea is to decrease the reference count of the underlying object
   * in an atomic manner. If the object reaches zero we delete it. With the deletion
   * there are some tricky semantics involved. For instance, if the object that is
   * deleted suddenly passes itself to another one then the reference count will be
   * increased again. However, the object was just being deleted, which means that
   * it should not be deleted again. There are two possible strategies to use this
   * file. Either you do care about using an object that is being deleted (leave 
   * everything as it is, the program will crash when you do so). Or you don't care
   * about it and really only want one deletion. In that case, uncomment the assert
   * in the incref method. In both cases, we detect a deletion operation by decreffing
   * the pointer once more.
   */
  void decref() const
  {
    if (ptr && ptr->decref()==0)
      {
	ptr->decref();
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

  bool operator==(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr==o.ptr;
    }
  bool operator!=(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr!=o.ptr;
    }
  bool operator<(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr<o.ptr;
    }
  bool operator<=(const Smart<ReferenceCountedObject>&o) const
    {
      return ptr<=o.ptr;
    }
  bool operator>=(const Smart<ReferenceCountedObject>&o) const
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
