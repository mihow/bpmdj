/****
 Om-Data
 Copyright (C) 2005-2006 Werner Van Belle

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

#ifndef OM_DATA_H
#define OM_DATA_H
#include <qstring.h>
#include <assert.h>
#include "version.h"
#include "types.h"

#ifndef WINDOWS
#ifndef LINUX

#ifdef Q_OS_WIN32
#define WINDOWS
#endif

#ifdef Q_OS_LINUX
#define LINUX
#endif

#endif
#endif

#ifndef WINDOWS
#ifndef LINUX
#error "Must define either LINUX or WINDOWS"
#endif
#endif

//===============================================================
//                            Data 
//===============================================================

//---------------------------------------------------------------
//                            Data
//---------------------------------------------------------------

/**
 * This is were the magic happens. All types in Om inherit from 
 * the DataClass. This class includes an allocation_count, which 
 * remembers how many time the object is supposed to be on the heap. 
 * This is similar to a reference counter. However, the first bump from
 * zero allocation count to 1 is performed by making an explcit shallow copy
 * towards the heap and returning that as value.
 */
class DataClass
{
 private:
  friend class Data;
  /** 
   * Normally an implicit
   * allocation has allocation_count == 0, indicating that it only exists in some temporary storage
   * As soon as a memory_copy is realized the allocation_count will increase. 
   */
  int allocation_count;

  /**
   * returns a copy to the dataclass object, by either making a shallowcopy 
   * (when going from allocation count 0 to 1) or by returning the this pointer. 
   * In all cases the allocation_count will be increased. The memory_copy and 
   * memory_release functions are only available from within the Data class, 
   * ensuring that a proper pointer management is possible.
   */
  DataClass* memory_copy() const;
  
  /**
   * decreases the allocation count. If it reaches zero, the object deletes itself with delete.
   * The memory_copy  and memory_release functions are only available from within the Data class, ensuring that
   * a proper pointer management is possible.
   */
  void       memory_release();
 protected:
  /**
   * The shallow_copy function should be overridden to return a 
   * shallow copy of the own object, but allocated with new.
   */
  virtual DataClass* shallow_copy() const = 0;
  virtual QString    type_name()    const = 0;
 public:

  /**
   * The constructors are overwritten to ensure a proper allocation_count. 
   */
  DataClass() : allocation_count(0) 
    {
    };
  
  DataClass(const DataClass&) : allocation_count(0) 
    {
    };

  DataClass & operator = (const DataClass&) 
    {
      return *this;
    };

  DataClass(const Data& d);
  virtual ~DataClass()
    {
      assert(allocation_count==0); 
    };
 protected:
  friend class DataVisitor;
  virtual void visit(DataVisitor& v) = 0;
};

/**
 * The class 'Data' represents a generic dataclass. It will take care
 * to count the references of the content it refers to and will deallocate
 * when the referencecounter reaches zero. It will also automatically copy
 * local allocated objects towards the heap when necessary. 
 * 
 * All data is shared explicitel, thus a dataclass can be passed along as if 
 * it were a pointer itself. The content will be explicitely shared. Data should
 * never be assigned in such a way that loops occur.
 *
 * The dataclass provides functions for easy downcasting, with automatically
 * included runtime type-check.
 * 
 * The data class is 
 * not supposed to have subclasses since it merely forms a wrapper around the
 * actual content, which should be subclassed from the class 'DataClass'
 */
class Data
{
 protected:
  friend class DataClass;

  /**
   * The actual content of this Data object. This is the only
   * field and is copied along. The Data class has ownership over this pointer
   * and will deallocate it as soon as the content its reference count 
   * reaches zero
   */
  DataClass * content;

  /**
   * called when a type error occurs. Currently this function
   * exits the program
   */
  void       type_error(QString expected) const;
 public:
  /**
   * Will create a data object with Null as contents
   */
  Data();

  /**
   * Will copy the data by taking a memory copy of the content.
   * If the content is not yet allcoated on the heap it will be copied
   * as well. The copy is only of the internal references. The data
   * objects behave explicitely shared.
   */
  Data(const Data& data);
  
  /**
   * Will take a copy of the given class, which can be anything, 
   * ranging from Null, Numbers, Floats, Arrays to Tokens...
   * The copy is only of the internal references. The data
   * objects behave explicitely shared.
   */
  Data(const DataClass & blah);

  /**
   * Will assign to this class by releasing the old content 
   * and refitting it with a pointer to the new. Refernce counts
   * are updated automatically.
   */
  Data & operator =(const Data& from);

  /**
   * Will decrease the reference count.
   */
  ~Data();
 public: 

  /**
   * Returns the type string of the data as a QString
   */
  QString type() const 
    {
      return content->type_name();
    };
  
  /**
   * true if the type of the content is Null
   * false otherwise
   */
  bool isNull() const;
  
  /**
   * Converts the data to a Symbol by passing it 
   * back as a Symbol value. If the type is not a symbol
   * a type_error is cast
   */
  operator Symbol() const;

  /**
   * Converts the data to a Token by passing it 
   * back as a Token value. If the type is not a symbol
   * a type_error is cast
   */
  operator Token() const;

  /**
   * Converts the data to a String by passing it 
   * back as a String value. If the type is not a symbol
   * a type_error is cast
   */
  operator String() const;

#define BASIC_TYPE(T,N,C) \
  operator C() const;
  BASIC_TYPES;
#undef BASIC_TYPE

#define ARRAY_TYPE(T,D) \
  operator Array<T,D>() const;
  ARRAY_TYPES;
#undef ARRAY_TYPE

  /**
   * Empty function at the moment. Will assert itself. This function is necessary to 
   * allow the array templates to compile properly
   */
  Data & operator +=(const Data &) 
    {
      assert(0); 
    };

  /**
   * Empty function at the moment. Will assert itself. This function is necessary to 
   * allow the array templates to compile properly
   */
  Data & operator -=(const Data &) 
    {
      assert(0); 
    };

  /**
   * Empty function at the moment. Will assert itself. This function is necessary to 
   * allow the array templates to compile properly
   */
  Data & operator /=(const Data &) 
    {
      assert(0); 
    };

  /**
   * Empty function at the moment. Will assert itself. This function is necessary to 
   * allow the array templates to compile properly
   */
  Data & operator *=(const Data &) 
    {
      assert(0); 
    };

  /**
   * Empty function at the moment. Will assert itself. This function is necessary to 
   * allow the array templates to compile properly
   */
  bool & operator !=(const Data &)
    {
      assert(0); 
    };

  /**
   * Empty function at the moment. Will assert itself. This function is necessary to 
   * allow the array templates to compile properly
   */
  bool & operator ==(const Data &) 
    {
      assert(0); 
    };
 protected:
  friend class DataVisitor;
  void visit(DataVisitor& v);
};

#endif
