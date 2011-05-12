#ifndef OM_SYMBOL_H
#define OM_SYMBOL_H
#include <qstring.h>
#include "data.h"

//---------------------------------------------------------------
//                            Symbol
//---------------------------------------------------------------
// symbols are represented as strings. Their
// unique quality is to be found in the quick
// comparators due to storing all symbols in a 
// symboltable. The actual order might not be 
// alphabetically.
class symbolLesser;
class Symbol: public DataClass
{
 private:
  QString *text;
  Symbol(QString * s) : text(s)
    {
    }
 protected:
  virtual DataClass* shallow_copy() const
    {
      return new Symbol(text);
    };
  virtual QString type_name() const 
    {
      return type(); 
    };
  virtual void visit(DataVisitor& v);
 public:
  static QString type()
    {
      return "Symbol";
    }
  Symbol() :  text(NULL) 
    {
    };
  Symbol(QString s);
  Symbol(const Symbol& s) : DataClass(), text(s.text)
    {
    }; 
  Symbol &operator =(const Symbol &s) 
    {
      text=s.text;
      return *this;
    };
  QString& operator ()() const 
    {
      return *text; 
    };
  operator QString&() const
    {
      return *text;
    }
  int length() const 
    {
      return text->length();
    };
  bool operator <   (const Symbol & other);
  bool operator ==  (const Symbol & other);
};

struct symbolLesser
{
  bool operator()(Symbol s1, Symbol s2) const 
  {
    return s1<s2;
  };
};

#endif
