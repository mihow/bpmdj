/****
 Hierarchical Data Objects
 Copyright (C) 2005-2011 Werner Van Belle
 http://flow.yellowcouch.org/data/

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
#ifndef __loaded__expr_test_cpp__
#define __loaded__expr_test_cpp__
using namespace std;
#line 1 "expr-test.c++"
#include <iostream>
#include <stdlib.h>
#include <qstring.h>
#include <typeinfo>
#include <map>
#include "expr.h"
namespace BorgIv{
  /**
   * This is a small test on our expression system. 
   * we work with strings and a map
   */
  class StringKey: public QString, public BorgIv::Key
  {
  public:
    StringKey(const char* k): QString(k)
    {
    }
  };
  
  class StringData: public BorgIv::ExpData, public QString
  {
  public:
    StringData(QString k): QString(k)
    {
    }
    virtual ExpData* new_copy() const
    {
      return new StringData(*this);
    }
    virtual Exp operator[](Key*key) const
    {
      assert(0);
      return Exp();
    }
  };

  class WriteExp: public Exp
  {
  public:
    WriteExp(Exp& e): Exp(e)
    {
    }
    void operator =(QString s)
    {
      Exp::operator=(new StringData(s));
    }
  };

  class TokenData: public ExpData
  {
    map<QString,Exp> content;
  protected:
    virtual ExpData* new_copy() const
    {
      return new TokenData(*this);
    }
  public:
    virtual Exp operator[](Key*key) const
    {
      StringKey* k = dynamic_cast<StringKey*>(key);
      if (content.find(*k)==content.end()) return Exp();
      else return content.find(*k)->second;
    }
    WriteExp operator[](QString k)
    {
      return content[k];
    }
};
 
} // namespace

using namespace BorgIv;
int main(int argc, char* argv[])
{
  TokenData token;
  token["Test"]="10";
  Exp expr(token);
  StringKey k("test");
  cout << ((StringData*)expr[&k])->toStdString();
  // a mess  ...
}

#endif // __loaded__expr_test_cpp__
