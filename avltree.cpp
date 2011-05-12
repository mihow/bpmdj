/****
 BpmDj: Free Dj Tools
 Copyright (C) 1995-2006 Werner Van Belle

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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "avltree.h"

#define LEFT_NODE(watte) (watte->left)
#define RIGHT_NODE(watte) (watte->right)
#define SET_LEFT_NODE(watte,to) (watte->left=to)
#define SET_RIGHT_NODE(watte,to) (watte->right=to)
#define POSITIVE_FLAG(watte) (watte->flag == 1)
#define NEGATIVE_FLAG(watte) (watte->flag == -1)
#define ZERO_FLAG(watte) (watte->flag == 0)
#define SET_ZERO_FLAG(watte) (watte->flag = 0)
#define SET_POSITIVE_FLAG(watte) (watte->flag = 1)
#define SET_NEGATIVE_FLAG(watte) (watte->flag = -1)
#define NULL_NODE Node<Key>::Null

template <class Key> Sentinel<Key> * Node<Key>::Null = new Sentinel<Key>();

//---------------------------------------------------------
//   The Sentinel
//---------------------------------------------------------
template <class Key> class Sentinel: public Node<Key>
{
public:
  virtual int compareAddData(Node<Key>*) {assert(0); return 0;};
  virtual int compareSearchData(Key)     {assert(0); return 0;};
  virtual void       print()             {assert(0); return;};
  virtual void       print(int)          { return; };
  virtual int        depth()             { return 0; };
  virtual int        count()             { return 0; };
  virtual bool       isNull()            { return true; };
  virtual bool       valid()             { return true; };
  virtual Node<Key>* search(Key)         { return this; };
  virtual Node<Key>* RotateLeft(int&)    {assert(0); return 0;};  
  virtual Node<Key>* RotateRight(int&)   {assert(0); return 0;};
  virtual Node<Key>* LeftFreedom(Node<Key>* &, int&)      {assert(0); return 0;};
  virtual Node<Key>* RightFreedom(Node<Key>* &, int&)     {assert(0); return 0;};
  virtual Node<Key>* GetLeftFreedom(Node<Key>* &, int&)   {assert(0); return 0;};
  virtual Node<Key>* GetRightFreedom(Node<Key>* &, int&)  {assert(0); return 0;};
  virtual Node<Key>* DeleteAvlNode(Key, int&)             { return this; };
  virtual Node<Key>* AddAvlNode(Node<Key>* who, int& depthchanged) { depthchanged=1; return who; };

};

//---------------------------------------------------------
//   Count calculation
//---------------------------------------------------------
template <class Key> int Node<Key>::count()
{
  return 1 + left->count() + right->count();
}

//---------------------------------------------------------
//   Depth calculation
//---------------------------------------------------------
template <class Key> int Node<Key>::depth()
{
  int l = left->depth();
  int r = right->depth();
  if (l>r) return l + 1;
  return r + 1;
}

//---------------------------------------------------------
//   Printing
//---------------------------------------------------------
template <class Key> void Node<Key>::print(int prefix)
{
  right->print(prefix + 1);
  for( int t = prefix; t > 0; t--) printf(".");
  print();
  int l = left->depth();
  int r = right->depth();
  printf(" left=%d, right=%d, count=%d ",l,r,count());
  // SANITY CHECK !!!
  if ( abs(l-r)>1
       || (l <  r  && ! flag >  0 ) 
       || (l >  r  && ! flag <  0 ) 
       || (l == r  && ! flag == 0 ) )
    printf("*********");
  printf("\n");
  left->print(prefix + 1);
}

//---------------------------------------------------------
//   Consistency validation
//---------------------------------------------------------
template <class Key> bool Node<Key>::valid()
{
  if (!left->valid()) return false;
  if (!right->valid()) return false;
  if (POSITIVE_FLAG(this))
    return left->depth() == right->depth() - 1 ;
  if (NEGATIVE_FLAG(this))
    return left->depth() == right->depth() + 1 ;
  return left->depth() == right->depth();
}


//---------------------------------------------------------
//   Search
//---------------------------------------------------------
template <class Key> Node<Key>* Node<Key>::search(Key watte)
{
  int compareResult = compareSearchData(watte);
  if (compareResult == 0 ) return this;
  if (compareResult == 1 ) return left->search(watte);
  if (compareResult == -1) return right->search(watte);
  return NULL_NODE;
}

//---------------------------------------------------------
//   Add
//---------------------------------------------------------

#define LEFT(WHO) WHO(Left,Right,LEFT_NODE,RIGHT_NODE,SET_LEFT_NODE,SET_RIGHT_NODE,\
	      NEGATIVE_FLAG,POSITIVE_FLAG,SET_NEGATIVE_FLAG,SET_POSITIVE_FLAG)
#define RIGHT(WHO) WHO(Right,Left,RIGHT_NODE,LEFT_NODE,SET_RIGHT_NODE,SET_LEFT_NODE,\
	      POSITIVE_FLAG,NEGATIVE_FLAG,SET_POSITIVE_FLAG,SET_NEGATIVE_FLAG)
#define CREATE_ROTATE(lname,rname,LN,RN,SLN,SRN,NF,PF,SNF,SPF)\
template <class Key> Node<Key>* Node<Key>::Rotate##lname(int& depthchanged)\
        {\
        Node<Key>* sec;\
	sec=RN(this);\
	assert(PF(this));\
	if (!(NF(sec)))\
	  {\
	     SRN(this,LN(sec));\
	     SLN(sec,this);\
	     if (PF(sec))\
	       {\
		  SET_ZERO_FLAG(this);\
		  SET_ZERO_FLAG(sec);\
		  depthchanged=1;\
		  return sec;\
	       }\
	     SPF(this);\
	     SNF(sec);\
	     return sec;\
	  }\
	  {\
	     Node<Key>* third=LN(sec);\
	     SLN(sec,RN(third));\
	     SRN(this,LN(third));\
	     SRN(third,sec);\
	     SLN(third,this);\
	     depthchanged=1;\
	     if (NF(third))\
	       {\
		  SPF(sec);\
		  SET_ZERO_FLAG(this);\
		  SET_ZERO_FLAG(third);\
		  return third;\
	       }\
	     if (ZERO_FLAG(third))\
	       {\
		  SET_ZERO_FLAG(sec);\
		  SET_ZERO_FLAG(third);\
		  SET_ZERO_FLAG(this);\
		  return third;\
	       }\
	     SET_ZERO_FLAG(sec);\
	     SNF(this);\
	     SET_ZERO_FLAG(third);\
	     return third;\
	  }\
     }

LEFT(CREATE_ROTATE);
RIGHT(CREATE_ROTATE);

#define CREATE_HANG(lname,rname,LN,RN,SLN,SRN,NF,PF,SNF,SPF) {\
	SLN(this,LN(this)->AddAvlNode(who,heightchanged));\
	if (heightchanged) {\
	     if (ZERO_FLAG(this)) {\
		  SNF(this);\
		  depthchanged=1;\
		  return this; }\
	     if (PF(this)) {\
		  SET_ZERO_FLAG(this);\
		  return this; }\
	     heightchanged=0;\
	     Node<Key> *tmp = Rotate##rname(heightchanged);\
	     if (!heightchanged) depthchanged=1;\
	     return tmp; }\
	return this; }

template <class Key> Node<Key>* Node<Key>::AddAvlNode(Node<Key>* who, int& depthchanged)
{
  int heightchanged=0;
  int CompareResult=compareAddData(who);
  assert(CompareResult); // this means 'no duplicates'
  if (CompareResult > 0) LEFT(CREATE_HANG);
  RIGHT(CREATE_HANG);
}


//---------------------------------------------------------
//   Deletion
//---------------------------------------------------------
#define CREATE_FREEDOM(lname,rname,LN,RN,SLN,SRN,NF,PF,SNF,SPF)\
template <class Key> Node<Key>* Node<Key>::lname##Freedom (Node<Key>* &placetostorefreedom,int &heightchanged) {\
	if (RN(this)->isNull()) \
           {\
	     if ((LN(this))->isNull()) {\
		  placetostorefreedom=this;\
		  heightchanged=1;\
		  return NULL_NODE; }\
	     heightchanged=1;\
	     placetostorefreedom=this;\
	     return LN(this); \
           }\
           {\
	     int takverkleind=0;\
	     SRN(this, RN(this) -> lname##Freedom (placetostorefreedom,takverkleind));\
	     if (takverkleind) \
	       {\
		  if (NF(this)) return Rotate##rname (heightchanged);\
		  if (PF(this))\
		    {\
		       SET_ZERO_FLAG(this);\
		       heightchanged=1;\
		       return this;\
		    }\
		  SNF(this);\
		  return this;\
	       }\
	     return this;\
	  }\
     }\
\
template <class Key> Node<Key>* Node<Key>::Get##lname##Freedom(Node<Key>* &placetostorefreedom,int &heightchanged)\
     {\
	Node<Key>* lname;\
	assert(LN(this));\
	lname = LN(this);\
	if (RN( lname ))\
	  return lname -> lname##Freedom (placetostorefreedom, heightchanged);\
	heightchanged=1;\
	placetostorefreedom= lname;\
	return LN( lname );\
     }

LEFT(CREATE_FREEDOM)
RIGHT(CREATE_FREEDOM)

#define CREATE_ZAK(lname,rname,LN,RN,SLN,SRN,NF,PF,SNF,SPF)\
SLN(this,LN(this)->DeleteAvlNode(wie,takkleiner));\
if (takkleiner) {\
	if (NF(this)) {\
	     SET_ZERO_FLAG(this);\
	     heightchanged=1;\
	     return this; }\
	if (ZERO_FLAG(this)) {\
	     SPF(this);\
	     return this; }\
	return Rotate##lname (heightchanged); }\
return this;
	  
template <class Key> Node<Key>* Node<Key>::DeleteAvlNode(Key wie, int& heightchanged)
{
  int CompareResult,takkleiner=0;
  CompareResult=this->compareSearchData(wie);
  switch (CompareResult)
    {
    case -1: /* this<wie */ RIGHT(CREATE_ZAK);
    case 1: /* this>wie */ LEFT(CREATE_ZAK);
    case 0: /* this==wie */
      /* nu een opnderscheid maken voor welke kinderen de 
       * this heeft */
      if (LEFT_NODE(this)->isNull() && RIGHT_NODE(this)->isNull())
	{
	  heightchanged=1;
	  delete this;  // WVB !! Dangerours ???
	  return NULL_NODE;
	}
      if (LEFT_NODE(this)->isNull())
	{
	  Node<Key>* rechter;
	  rechter=RIGHT_NODE(this);
	  assert(POSITIVE_FLAG(this));
	  assert(rechter);
	  assert(LEFT_NODE(rechter)->isNull());
	  assert(RIGHT_NODE(rechter)->isNull());
	  heightchanged=1;
	  delete this; // WVB -- dangerous
	  return rechter;
	}
      if (RIGHT_NODE(this)->isNull())
	{
	  Node<Key>* linker;
	  linker=LEFT_NODE(this);
	  assert(NEGATIVE_FLAG(this));
	  assert(linker);
	  assert(LEFT_NODE(linker)->isNull());
	  assert(RIGHT_NODE(linker)->isNull());
	  heightchanged=1;
	  delete this;  // WVB -- dangerours ???
	  return linker;
	}
      if (POSITIVE_FLAG(this))
	{
	  Node<Key>* freedom;
	  SET_RIGHT_NODE(this,GetRightFreedom(freedom,heightchanged));
	  SET_LEFT_NODE(freedom,LEFT_NODE(this));
	  SET_RIGHT_NODE(freedom,RIGHT_NODE(this));
	  if (heightchanged) SET_ZERO_FLAG(freedom);
	  else SET_POSITIVE_FLAG(freedom);
	  delete this;   // WVB -- dangerous ???
	  return freedom;
	}
      if (NEGATIVE_FLAG(this))
	{
	  Node<Key>* freedom;
	  SET_LEFT_NODE(this,GetLeftFreedom(freedom,heightchanged));
	  SET_LEFT_NODE(freedom,LEFT_NODE(this));
	  SET_RIGHT_NODE(freedom,RIGHT_NODE(this));
	  if (heightchanged) SET_ZERO_FLAG(freedom);
	  else SET_NEGATIVE_FLAG(freedom);
	  delete this;  // WVB -- dangerours ??
	  return freedom;
	}
      if (ZERO_FLAG(this))
	{
	  Node<Key>* freedom;
	  int takverkleind=0;
	  SET_LEFT_NODE(this,GetLeftFreedom(freedom,takverkleind));
	  SET_LEFT_NODE(freedom,LEFT_NODE(this));
	  SET_RIGHT_NODE(freedom,RIGHT_NODE(this));
	  if (takverkleind) SET_POSITIVE_FLAG(freedom);
	  else  SET_ZERO_FLAG(freedom);
	  delete this;   // WVB -- dangerours
	  return freedom;
	}
    default: exit(57);
    }
}

//---------------------------------------------------------
//   Constructor and Wrappers
//---------------------------------------------------------

template <class Key> Node<Key>::Node()
{
  left = NULL;
  right = NULL;
}

template <class Key> AvlTree<Key>::AvlTree()
{
  init(); 
};

template <class Key> void AvlTree<Key>::init()
{
  root = Node<Key>::Null; 
};

template <class Key> void AvlTree<Key>::PurgeAll(Node<Key>* root)
{
  if (root && !root->isNull())
    {
      PurgeAll(root->left);
      PurgeAll(root->right);
      delete(root);
    }
}

template <class Key> AvlTree<Key>::~AvlTree()
{
  purge();
};

template <class Key> Node<Key>* AvlTree<Key>::AddAvlNode(Node<Key>* root, Node<Key>* who)
{
  if (who->isNull()) return root;
  who->left = NULL_NODE;
  who->right = NULL_NODE;
  who->flag = 0;
  int depthchanged=0;
  return root->AddAvlNode(who,depthchanged);
}

template <class Key> Node<Key>* AvlTree<Key>::DeleteAvlNode(Node<Key>* root, Key wie)
{
  int heightchanged=0;
  return root->DeleteAvlNode(wie,heightchanged);
}
