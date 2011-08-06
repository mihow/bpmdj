/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__qt_helpers_h__
#define __loaded__qt_helpers_h__
using namespace std;
#include <qtreewidget.h>
#include <qstring.h>
#include <vector>
#include <assert.h>
#include <qmenu.h>
#include "qsong.h"
#include "iterator.h"

/**
 * An iterator to run over a treeWidget
 */
template<class T>
class treeWidgetIterator
{
  QTreeWidgetItemIterator it;
  bool once;
public:
  inline void key()
  {
    assert(0);
  }
  inline T* val()
  {
    return (T*)*it;
  }
  inline bool valid()
  {
    return *it;
  }
  inline void step()
  {
    ++it;
  }
  inline bool prepare_block()
  {
    if (once) return false;
    once=true;
    return valid();
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  treeWidgetIterator(QTreeWidgetItem* C): it(C), once(false)
  {
  };
  treeWidgetIterator(QTreeWidget* C): it(C), once(false)
  {
  };
};

typedef treeWidgetIterator<QTreeWidgetItem> stdTreeWidgetIterator;

/**
 * Because some moron at Trolltech has no idea how to parse a type
 * we can no longer use the unsigned1, unsigned2, unsigned4 and unsigned8
 * types directly. Instead we need to resort to a silly workaround in which
 * we make sure that the bloody MOC compiler does not notice that we actually 
 * pass our favorite type along. Grmbl. I hate fools that think that they
 * can program and just go with 'let's just fix it like this'.
 * This said. We will be using wunsigned1 to 8 as typedefs for the other ones
 */
typedef unsigned8 wunsigned8;
typedef unsigned4 wunsigned4;
typedef unsigned2 wunsigned2;
typedef unsigned1 wunsigned1;

/**
 * The old Qt3 had a findItem function. In Qt4 this no longer exists, so 
 * we need to deal with it through a findItems (plural) form. 
 * which we did and also check the types directly
 */
template<class T>
T* findItem(QTreeWidget* lst, QString text, int pos)
{
  assert(lst);
  QList<QTreeWidgetItem*> items=lst->findItems(text,Qt::MatchExactly,pos);
  int l = items.count();
  if (l==0) return NULL;
  if (l==1) return dynamic_cast<T*>(items[0]);
  assert(l<2);
  return NULL;
}

template<class T>
T* findOneItem(QTreeWidget* lst, QString text, int pos)
{
  assert(lst);
  QList<QTreeWidgetItem*> items=lst->findItems(text,Qt::MatchExactly,pos);
  int l = items.count();
  assert(l==1);
  assert(items[0]);
  return dynamic_cast<T*>(items[0]);
}

inline QTreeWidgetItem* findStdItem(QTreeWidget* lst, QString text, int pos)
{
  return findItem<QTreeWidgetItem>(lst,text,pos);
}

inline QTreeWidgetItem* findOneStdItem(QTreeWidget* lst, QString text, int pos)
{
  return findOneItem<QTreeWidgetItem>(lst,text,pos);
}

void selectAll(QTreeWidget* lst, bool yes);

template<class T>
T* lastItem(QTreeWidget* lst)
{
  assert(lst);
  int l=lst->topLevelItemCount();
  if (l==0) return NULL;
  QTreeWidgetItem* last=lst->topLevelItem(l-1);
  return dynamic_cast<T*>(last);
}

template<class T>
T* firstItem(QTreeWidget* lst)
{
  assert(lst);
  int l=lst->topLevelItemCount();
  if (l==0) return NULL;
  QTreeWidgetItem* first=lst->topLevelItem(0);
  return dynamic_cast<T*>(first);
}

/**
 * Grunt, what a nice function this was... until they removed it
 */
template<class T> 
T* nextSibling(T* lst)
{
  assert(lst);
  // first find the parent.
  QTreeWidgetItem* parent=lst->parent();
  assert(parent);
  // then find the index of this element
  int index=parent->indexOfChild(lst);
  // check whether we have sufficent children
  int l=parent->childCount();
  // advance
  index++;
  if (index>=l) return NULL;
  // return the next one
  QTreeWidgetItem* answer=parent->child(index);
  return dynamic_cast<T*>(answer);
}

template<class T>
struct AssociatedAction: public QAction
{
  T* id;
  AssociatedAction(T* _i, QString text): QAction(text, NULL), id(_i)
  {
  }
};

QColor mixColor(QColor a, float4 d, QColor b);

void setBackgroundColor(QWidget* widget, QColor color);
QColor backgroundColor(QWidget* widget);
float4 qtof4(QString str);
float8 qtof8(QString str);
signed4 qtos4(QString str);
#endif // __loaded__qt_helpers_h__
