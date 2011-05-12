/****
 BpmDj: Free Dj Tools
 Copyright (C) 1995-2005 Werner Van Belle

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

/*----------------------------------------------------------------------------
 * 
 * 
 *                                AVLTREE 1.0
 *                       (c) Werner Van Belle aug 95
 *
 *
 * NODE_ADT AddAvlNode(NODE root, NODE who)
 *   Voegt who toe aan root, geeft als resultaat de nieuwe root weer
 *   O(log2n)
 * 
 * NODE SearchAvlNode(NODE root, SEARCH_DATA watte)
 *   Zoekt watte op in root, geeft als resultaat de bijhorende node weer
 *   O(<log2n)
 * 
 * NODE DeleteAvlNode(NODE root, SEARCH_DATA wie)
 *   Verwijdert wie uit root, geeft als resultaat de nieuwe root weer
 *   O(log2n)
 * 
 * int ValidAvlTree(Node* root)
 *   Controleert of de AVL-tree wel aan de AVL-voorwaarde voldoet. Hier 
 *   wordt niet gecontroleerd of de linkerkinderen kleiner zijn dan root 
 *   en de rechterkinderen groter zijn dan root 
 *   O(n)
 * 
 * int AvlTreeCount(Node* root)
 *   geeft weer hoeveel elementen in de tree steken. 
 *   O(n)
 * 
 * int depth()
 *   geeft de maximum diepte van een pad doorheen de tree weer. 
 *   O(n)
 *
 * void PrintAvlTree(Node* root, int diepte)
 *   Print de avltree. Deze functie is enkel aanwezig als PRINT_NODE
 *   gedefinieerd is. Het output formaat is 
 *     .rechterkind
 *     rootzelf
 *     .linkerkind
 */

#ifndef AVLTREE_H
#define AVLTREE_H
#include <assert.h>

template <class Key> class Sentinel;
template <class Key> class Node
{
 public:
   static Sentinel<Key> * Null;
   Node<Key> * left;
   Node<Key> * right;
   int       flag;
   Node<Key>();
   virtual ~Node<Key>() {};
   virtual int compareAddData(Node<Key>* w2) = 0; // compares this with w2
   virtual int compareSearchData(Key key) = 0;    // compares this with key
   virtual void print() {assert(0);};
   virtual void print(int prefix);
   virtual int  depth();
   virtual int  count();
   virtual bool valid();
   virtual Node<Key>* search(Key);
   virtual bool isNull()       {return false;};
   virtual Node<Key>* RotateLeft(int& depthchanged);
   virtual Node<Key>* RotateRight(int& depthchanged);
   virtual Node<Key>* LeftFreedom(Node<Key>* &placetostorefreedom, int& heightchanged);
   virtual Node<Key>* RightFreedom(Node<Key>* &placetostorefreedom, int& heightchanged); 
   virtual Node<Key>* GetLeftFreedom(Node<Key>* &placetostorefreedom, int& heightchanged);
   virtual Node<Key>* GetRightFreedom(Node<Key>* &placetostorefreedom, int& heightchanged);
   virtual Node<Key>* AddAvlNode(Node<Key>* who, int& depthchanged);
   virtual Node<Key>* DeleteAvlNode(Key wie, int& heightchanged);
   Node<Key>* toAnswer() {return isNull() ? NULL : this;};
};


template <class Key> class AvlTree 
{
 private:
  Node<Key>* root;
  Node<Key>* AddAvlNode(Node<Key>* root, Node<Key>* who);
  Node<Key>* DeleteAvlNode(Node<Key>* root, Key wie);
  void PurgeAll(Node<Key>* root);
 public:
  AvlTree();
  void init();
  virtual ~AvlTree();
  void add(Node<Key>* who)     { root = AddAvlNode(root,who); };
  Node<Key>* search(Key watte) { return root->search(watte)->toAnswer();};
  void del(Key watte)          { root = DeleteAvlNode(root,watte); };
  bool valid()                 { return root->valid(); };
  int count()                  { return root->count(); };
  int depth()                  { return root->depth(); };
  void print()                 { root->print(0); };
  void purge()                 { PurgeAll(root); root = NULL; };
  Node<Key> *top()             { return root->toAnswer(); };
};

#endif
