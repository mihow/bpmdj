#ifndef OM_STRING_H
#define OM_STRING_H
#include "data.h"
//---------------------------------------------------------------
//                            String
//---------------------------------------------------------------
class String: public DataClass
{
  QString text;
 protected:
  virtual QString type_name() const 
    {
      return type(); 
    };
  virtual DataClass* shallow_copy() const 
    {
      return new String(text);
    };
 public:
  static QString type()
    {
      return "String";
    }
  String(QString s) : text(s) 
    {
    };
  QString operator ()() const 
    {
      return text;
    };
  operator QString() const 
    {
      return text;
    }
  static String parse(const char *);
 protected:
  virtual void visit(DataVisitor& v);
};

#endif
