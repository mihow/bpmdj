/****
 Borg IV compiled file
 Copyright (C) 2006-2007 Werner Van Belle

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

#ifndef __FRAGMENT_CREATOR_H
#define __FRAGMENT_CREATOR_H
#include "active-objects.h"
#include "reference-count.h"
using namespace std;
#include <deque>
#include "fragment-cache.h"
class FragmentCreator;
class ActiveFragmentCreator;
#ifdef TRACE_MESSAGES
#define ENTER_MSG cerr << "Start " << declaration() << "\n";
#define LEAVE_MSG cerr << "Stop " << declaration()  << "\n";
#endif
#ifndef ENTER_MSG
#define ENTER_MSG ;
#endif
#ifndef LEAVE_MSG
#define LEAVE_MSG ;
#endif

//-------------------------------------
// Active Object base messaging classes
//-------------------------------------
class ActiveFragmentCreator_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveFragmentCreator * /* caller */)
    {
      assert(0);
    }
    virtual string declaration()
    {
      return "Unknown message";
    }
};


//-------------------------------------
// Main object definition
//-------------------------------------
class ActiveFragmentCreator: public ActiveObject<Smart< ActiveFragmentCreator_msg_ > >
{
  friend class FragmentCreator;
    virtual elementResult handle(Smart< ActiveFragmentCreator_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  deque < FragmentCreated >  ready;
  public: elementResult createOneFor(Song* song);
  protected: void queue_createOneFor(Song* song);
  public: elementResult terminate();
  protected: void queue_terminate();
  bool handle();
  protected:
    ActiveFragmentCreator(string name="FragmentCreator"):
      ActiveObject<Smart< ActiveFragmentCreator_msg_ > >(name)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveFragmentCreator_msg_createOneFor: public ActiveFragmentCreator_msg_
{
    Song* song;
  public:
    ActiveFragmentCreator_msg_createOneFor(Song* song) : song(song)
      {
      };
    virtual string declaration()
    {
      return "FragmentCreator::createOneFor(Song* song)";
    }
    virtual elementResult run(ActiveFragmentCreator * ao)
      {
        ENTER_MSG;
        elementResult res = ao->createOneFor(song);
        LEAVE_MSG;
        return res;
      };
};

class ActiveFragmentCreator_msg_terminate: public ActiveFragmentCreator_msg_
{
    ;
  public:
    ActiveFragmentCreator_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "FragmentCreator::terminate()";
    }
    virtual elementResult run(ActiveFragmentCreator * ao)
      {
        ENTER_MSG;
        elementResult res = ao->terminate();
        LEAVE_MSG;
        return res;
      };
};


//-------------------------------------
// Active Object wrapper 
//-------------------------------------
class FragmentCreator
{
  private:
    ActiveFragmentCreator object;
  public:
    void createOneFor(Song* song)
    {
      object.queue_createOneFor(song);
    };
  public:
    void terminate()
    {
      object.queue_terminate();
    };
  public:
    deque < FragmentCreated >  getReadyOnes();
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveFragmentCreator::queue_createOneFor(Song* song)
  {
    push(Smart<ActiveFragmentCreator_msg_createOneFor>(
        new ActiveFragmentCreator_msg_createOneFor(song)));
  };
inline void ActiveFragmentCreator::queue_terminate()
  {
    push(Smart<ActiveFragmentCreator_msg_terminate>(
        new ActiveFragmentCreator_msg_terminate()));
  };
#endif // __FRAGMENT_CREATOR_H
