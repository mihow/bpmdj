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
#ifndef __loaded__expr_h__
#define __loaded__expr_h__
using namespace std;
#line 1 "expr.h++"
#include "reference-count.h"
namespace BorgIv{
  class ExpData;
  
  class Key
  {
  public:
    virtual ~Key()
    {
    };
  };

  /**
   * @ingroup data
   * @brief Functional expressions in C++
   *
   * Expressions are split in two parts. The first part are the Exp and @ref WriteExp objects, which represent 
   * the main access point to read only and writable expressions. The second side of expressions is the logic behind it, 
   * which is hidden away in the ExpContent. All expressions can be indexed (if they are array like, otherwise an error
   * can be thrown or nothing returned. Depending on the writability the returned value can be written to.
   *
   * To use the Exp, ReadExp and WriteExp objects with the memory architecture we need to provide first the basic
   * ExpContents which are in our case the Map and File objects. Aside from this do we need the standard value_id'ed
   * object. Normally every exp will contain such a ValueIdContent. If we make a map writable then we convert it 
   * to a writable object in which 
   */
  class Exp: public Smart<ExpData>, public Key
  {
  public:
    Exp();
    Exp(ExpData * d);
    /**
     * Will make a copy of the input expression
     */ 
    Exp(ExpData& scratch);
    /**
     * @name Indexing expressions
     * Indexing in expression always results in read only data. This avoids a destructive programming style. 
     * To modify data, create a new expression containing a specific set of tokens (in whatever native type 
     * that is available) and cast it to an expression itself.  Another way to create new expressions
     * in a destructive way is to use ExpCreator, which  will not return read only values but assignable 
     * expressions. 
     */
    Exp operator[](Key* key) const;
    /**
     * type conversion to our expected target type.
     */
    template <class TargetType> 
    operator const TargetType() const
    {
      return dynamic_cast<const TargetType>(ptr);
    }

    /**
     * assignment 
     */
    void operator =(ExpData* data)
    {
      Smart<ExpData>::operator=((const Smart<ExpData>)data);
    }
  };
  
  /**
   * @brief provides the 'behind-the-expr' logic of expressions
   * 
   * The content of an expression can be anything as long as it has the proper operations to index is present.
   * The expdata must always be treatable as a constant when residing in an  @ref Exp. ExpContent objects are 
   * automatically reference counted, which should always work since we should not make circular data structures 
   * at all.
   */
  class ExpData: public ReferenceCount
  {
  protected:
    friend class Exp;
    /**
     * must return a perfect copy of this. Is used by @ref copy_if_necessary
     * when an object is moved from scratch space to more permanent space
     */
    virtual ExpData* new_copy() const = 0;
    virtual Exp operator[](Key* key) const = 0;
    /**
     * if the reference count == 0, then this object will be recreated with the
     * copy operator, otherwise this will be returned;
     */
    ExpData* copy_if_necessary() 
    {
      if (reference_count==0) return new_copy();
      return this;
    }
  };

  Exp::Exp(ExpData * d): Smart<ExpData>(d->copy_if_necessary())
  {
    assert(d);
  }

  Exp::Exp(ExpData& scratch): Smart<ExpData>(scratch.copy_if_necessary())
  {
  }

  Exp Exp::operator[](Key* key) const
  {
    return ptr[0][key];
  }
  
  Exp::Exp(): Smart<ExpData>()
  {
  };
  
} // end namespace
#endif // __loaded__expr_h__
