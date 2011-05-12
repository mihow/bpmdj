#ifndef OM_DATA_H
#define OM_DATA_H
#include <qstring.h>
#include "types.h"

//===============================================================
//                            Data 
//===============================================================
// All data is shared explicitely. When copying data the goal is 
// to always refer to the same piece of data. This brings
// some interesting challenges with it, among other, the need
// for deep copying data. 

//---------------------------------------------------------------
//                            Data
//---------------------------------------------------------------
// the constructors are overwritten to ensure
// a proper allocation_count. Normally an implicit
// allocation has allocation_count == 0, indicating
// that it only exists in some temporary storage
// As soon as a memory_copy is realized the
// allocation_count will increase. The memory_copy 
// and memory_release functions are only available 
// from within the Data class, ensuring that
// a proper pointer management is possible.
// the shallow_copy function should be overridden 
// to return a copy of the own object.
class DataClass
{
 private:
  friend class Data;
  int allocation_count;
  DataClass* memory_copy() const;
  void       memory_release();
 protected:
  virtual DataClass* shallow_copy() const = 0;
  virtual QString    type_name()    const = 0;
 public:
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

// The data type represents a poymorphic piece
// of data: a dataclass or one of its subclasses.
// The data class can be passed along as if it
// were a pointer itself. The content will always
// point to the same thing. If a temporary object
// is passed into a data structure then it will be
// automatically copied onto the heap.
class Data
{
 private:
  friend class DataClass;
  DataClass * content;
  void       type_error(QString expected) const;
 public:
  Data();
  Data(const Data& data);
  Data(const DataClass & blah);
  Data & operator =(const Data& from);
  ~Data();
 public: 
  // type checking
  QString type() const 
    {
      return content->type_name();
    };
  bool isNull() const;
  // conversion operators
  operator Symbol() const;
  operator Token() const;
  operator String() const;
#define BASIC_TYPE(T,N,C) \
  operator C() const;
  BASIC_TYPES;
#undef BASIC_TYPE

#define ARRAY_TYPE(T,D) \
  operator Array<T,D>() const;
  ARRAY_TYPES;
#undef ARRAY_TYPE

  // general purpose operators necessary to
  // allow the array templates to compile properly
  Data & operator +=(const Data &) 
    {
      assert(0); 
    };
  Data & operator -=(const Data &) 
    {
      assert(0); 
    };
  Data & operator /=(const Data &) 
    {
      assert(0); 
    };
  Data & operator *=(const Data &) 
    {
      assert(0); 
    };
  bool & operator !=(const Data &)
    {
      assert(0); 
    };
  bool & operator ==(const Data &) 
    {
      assert(0); 
    };
 protected:
  friend class DataVisitor;
  void visit(DataVisitor& v);
};

#endif
