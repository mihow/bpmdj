#ifndef TOKEN_H
#define TOKEN_H
#include <vector>
#include <map>
#include "symbol.h"
using namespace std;

//---------------------------------------------------------------
//                            Token
//---------------------------------------------------------------
class Token: public DataClass
{
  struct TokenContent
  {
    map<Symbol, Data, symbolLesser> content;
    int refcount;
    TokenContent() : content() 
    {
      refcount = 1;
    };
  } * content;
 protected:
  virtual QString type_name() const 
    {
      return type(); 
    };
  virtual DataClass* shallow_copy() const 
    {
      return new Token(*this);
    };
  virtual void visit(DataVisitor&v);
 public:
  static QString type() 
    {
      return "Token";
    }
  Token();
  Token(const Token& dc);
  Token& operator = (const Token& dc);
  virtual ~Token();

  int count() const 
    {
      return content->content.size(); 
    };
  void define(Symbol key, Data val) 
    {
      content->content[key]=val;
    };
  Data get(const string s) 
    {
      return content->content[Symbol(s)]; 
    };
  Data& operator[](const string s) 
    {
      return content->content[Symbol(s)]; 
    };
  Data& operator[](Symbol s) 
    {
      return content->content[s]; 
    };
  Data& operator[](Symbol *s) 
    {
      return content->content[*s]; 
    };
  vector<Symbol> keys() const;
};

#endif
