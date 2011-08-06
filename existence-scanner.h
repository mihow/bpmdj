/****
 Active Object compiled file
 Copyright (C) 2006-2011 Werner Van Belle
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

#ifndef __EXISTENCE_SCANNER_H
#define __EXISTENCE_SCANNER_H
#include "./Active/active-objects.h"
using namespace std;
#include <vector> 
#include "song.h"
class ExistenceScanner;
class ActiveExistenceScanner;
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
 * the true ActiveExistenceScanner. ActiveExistenceScanner_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveExistenceScanner_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub ExistenceScanner
 */
class ActiveExistenceScanner_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveExistenceScanner itself.
     */
    virtual elementResult run(ActiveExistenceScanner * /* caller */)
    {
      assert(0);
      return Revisit;
    }
    /**
     * Returns the name of this message. Since this is the message baseclass
     * it has no identity and will return 'Unknown Message' 
     */
    virtual string declaration()
    {
      return "Unknown message";
    }
    /**
     * The following is necessary so that child desctructors are invoked
     * when a message is deleted in the handle_message loop.
     */
    virtual ~ActiveExistenceScanner_msg_()
    {
    }
};


//-------------------------------------
// Main object definition
//-------------------------------------
class ActiveExistenceScanner: public ActiveObject< ActiveExistenceScanner_msg_* >
{
  friend class ExistenceScanner;
  ExistenceScanner * self;
    virtual elementResult handle( ActiveExistenceScanner_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  vector < Song* > * songs;
  unsigned current;
  public: elementResult start(vector < Song* > * all);
  protected: void queue_start(vector < Song* > * all);
  public: elementResult thunk();
  protected: void queue_thunk();
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveExistenceScanner(ExistenceScanner* s, string name):
      ActiveObject< ActiveExistenceScanner_msg_ * >(name), self(s)
      {
      current =  0;
      songs = NULL;
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveExistenceScanner_msg_start: public ActiveExistenceScanner_msg_
{
    vector < Song* > * all;
  public:
    ActiveExistenceScanner_msg_start(vector < Song* > * all) : all(all)
      {
      };
    virtual string declaration()
    {
      return "ExistenceScanner::start(vector < Song* > * all)";
    }
    virtual elementResult run(ActiveExistenceScanner * ao)
      {
        ENTER_MSG;
        elementResult res = ao->start(all);
        LEAVE_MSG;
        return res;
      };
};

class ActiveExistenceScanner_msg_thunk: public ActiveExistenceScanner_msg_
{
    ;
  public:
    ActiveExistenceScanner_msg_thunk()
      {
      };
    virtual string declaration()
    {
      return "ExistenceScanner::thunk()";
    }
    virtual elementResult run(ActiveExistenceScanner * ao)
      {
        ENTER_MSG;
        elementResult res = ao->thunk();
        LEAVE_MSG;
        return res;
      };
};

class ActiveExistenceScanner_msg_terminate: public ActiveExistenceScanner_msg_
{
    ;
  public:
    ActiveExistenceScanner_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "ExistenceScanner::terminate()";
    }
    virtual elementResult run(ActiveExistenceScanner * ao)
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
 * to an object subclassed from type ActiveExistenceScanner_msg_
 * The stub itself has an instance of the true active object.
 */
class ExistenceScanner
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveExistenceScanner object;
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
    ExistenceScanner(string name="ExistenceScanner"): object(this, name) {};
  public:
    void start(vector < Song* > * all)
    {
      object.queue_start(all);
    };
  public:
    void thunk()
    {
      object.queue_thunk();
    };
  public:
    void terminate()
    {
      object.queue_terminate();
    };
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveExistenceScanner::queue_start(vector < Song* > * all)
  {
    push(new ActiveExistenceScanner_msg_start(all));
  };
inline void ActiveExistenceScanner::queue_thunk()
  {
    push(new ActiveExistenceScanner_msg_thunk());
  };
inline void ActiveExistenceScanner::queue_terminate()
  {
    push(new ActiveExistenceScanner_msg_terminate());
  };
#endif // __EXISTENCE_SCANNER_H
