/****
 Active Object compiled file
 Copyright (C) 2006-2010 Werner Van Belle
 Do not modify. Changes might be lost
 --------------------------------------------
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
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
/**
 * Represents the basic message that is used to queue or deliver a call to 
 * the true ActiveFragmentCreator. ActiveFragmentCreator_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveFragmentCreator_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub FragmentCreator
 * The message class is also an instance of ReferenceCount, which makes it 
 * ideally suited to use within Smart pointers.
 */
class ActiveFragmentCreator_msg_: public ReferenceCount
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveFragmentCreator itself.
     */
    virtual elementResult run(ActiveFragmentCreator * caller)
    {
      assert(0);
    }
    /**
     * Returns the name of this message. Since this is the message baseclass
     * it has no identity and will return 'Unknown Message' 
     */
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
  FragmentCreator * self;
    virtual elementResult handle(Smart< ActiveFragmentCreator_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  deque < FragmentFile >  ready;
  map<Song*,FragmentFile> created;
  public: elementResult createOneFor(Song* song);
  protected: void queue_createOneFor(Song* song);
  public: elementResult terminate();
  protected: void queue_terminate();
  bool handle();
  protected:
    ActiveFragmentCreator(FragmentCreator* s, string name):
      ActiveObject<Smart< ActiveFragmentCreator_msg_ > >(name), self(s)
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
/**
 * Represents the stub that will transform each incoming call (method)
 * to an object subclassed from type ActiveFragmentCreator_msg_
 * The stub itself has an instance of the true active object.
 */
class FragmentCreator
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveFragmentCreator object;
  public:
    /**
     * The constructor of the stub will also directly initalize the main
     * object. Because object construction and delayed calls interfere
     * somewhat we prohibit the actual implementation (and especially use)
     * of a specialized Active Object constructor. Instead, simply the name
     * is passed to the object. If you need to initialize the stub, you
     * should consider adding an init message to the active object and
     * calling (well sending a message to) it directly when the stub is
     * generated.
     */
    FragmentCreator(string name="FragmentCreator"): object(this, name) {};
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
    deque < FragmentFile >  getReadyOnes();
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
