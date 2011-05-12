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

#ifndef __AO_TRACKER_H
#define __AO_TRACKER_H
#include "active-objects.h"
using namespace std;
#include <set>
#include <string>
class AoTracker;
class ActiveAoTracker;
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
 * the true ActiveAoTracker. ActiveAoTracker_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveAoTracker_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub AoTracker
 */
class ActiveAoTracker_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveAoTracker itself.
     */
    virtual elementResult run(ActiveAoTracker * /* caller */)
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
};


//-------------------------------------
// Main object definition
//-------------------------------------
class ActiveAoTracker: public ActiveObject< ActiveAoTracker_msg_* >
{
  friend class AoTracker;
  AoTracker * self;
    virtual elementResult handle( ActiveAoTracker_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  set < string >  alive;
  public: elementResult sunset(string s);
  protected: void queue_sunset(string s);
  public: elementResult sunrise(string s);
  protected: void queue_sunrise(string s);
  public: elementResult print();
  protected: void queue_print();
  protected:
    ActiveAoTracker(AoTracker* s, string name):
      ActiveObject< ActiveAoTracker_msg_ * >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveAoTracker_msg_sunset: public ActiveAoTracker_msg_
{
    string s;
  public:
    ActiveAoTracker_msg_sunset(string s) : s(s)
      {
      };
    virtual string declaration()
    {
      return "AoTracker::sunset(string s)";
    }
    virtual elementResult run(ActiveAoTracker * ao)
      {
        ENTER_MSG;
        elementResult res = ao->sunset(s);
        LEAVE_MSG;
        return res;
      };
};

class ActiveAoTracker_msg_sunrise: public ActiveAoTracker_msg_
{
    string s;
  public:
    ActiveAoTracker_msg_sunrise(string s) : s(s)
      {
      };
    virtual string declaration()
    {
      return "AoTracker::sunrise(string s)";
    }
    virtual elementResult run(ActiveAoTracker * ao)
      {
        ENTER_MSG;
        elementResult res = ao->sunrise(s);
        LEAVE_MSG;
        return res;
      };
};

class ActiveAoTracker_msg_print: public ActiveAoTracker_msg_
{
    ;
  public:
    ActiveAoTracker_msg_print()
      {
      };
    virtual string declaration()
    {
      return "AoTracker::print()";
    }
    virtual elementResult run(ActiveAoTracker * ao)
      {
        ENTER_MSG;
        elementResult res = ao->print();
        LEAVE_MSG;
        return res;
      };
};


//-------------------------------------
// Active Object wrapper 
//-------------------------------------
/**
 * Represents the stub that will transform each incoming call (method)
 * to an object subclassed from type ActiveAoTracker_msg_
 * The stub itself has an instance of the true active object.
 */
class AoTracker
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveAoTracker object;
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
    AoTracker(string name="AoTracker"): object(this, name) {};
  public:
    void sunset(string s)
    {
      object.queue_sunset(s);
    };
  public:
    void sunrise(string s)
    {
      object.queue_sunrise(s);
    };
  public:
    void print()
    {
      object.queue_print();
    };
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveAoTracker::queue_sunset(string s)
  {
    push(new ActiveAoTracker_msg_sunset(s));
  };
inline void ActiveAoTracker::queue_sunrise(string s)
  {
    push(new ActiveAoTracker_msg_sunrise(s));
  };
inline void ActiveAoTracker::queue_print()
  {
    push(new ActiveAoTracker_msg_print());
  };
#endif // __AO_TRACKER_H
