#ifndef OM_NULL_H
#define OM_NULL_H
#include "data.h"

//---------------------------------------------------------------
//                            Null
//---------------------------------------------------------------
class Null: public DataClass
{
 private:
  void crash() const;
 protected:
  virtual QString type_name() const 
    {
      return "Null"; 
    };
  virtual DataClass* shallow_copy() const 
    {
      return new Null();
    };
  virtual void visit(DataVisitor&) 
    {
      crash(); 
    };
 public:
  Null() 
    {
    };
};

#endif
