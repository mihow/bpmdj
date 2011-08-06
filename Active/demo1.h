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

#ifndef __DEMO1_H
#define __DEMO1_H
#include "active-objects.h"
using namespace std;
class DemoSender;
class ActiveDemoSender;
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
class DemoReceiver;
class ActiveDemoReceiver;
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
 * the true ActiveDemoSender. ActiveDemoSender_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveDemoSender_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub DemoSender
 */
class ActiveDemoSender_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveDemoSender itself.
     */
    virtual elementResult run(ActiveDemoSender * /* caller */)
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
    virtual ~ActiveDemoSender_msg_()
    {
    }
};

/**
 * Represents the basic message that is used to queue or deliver a call to 
 * the true ActiveDemoReceiver. ActiveDemoReceiver_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveDemoReceiver_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub DemoReceiver
 */
class ActiveDemoReceiver_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveDemoReceiver itself.
     */
    virtual elementResult run(ActiveDemoReceiver * /* caller */)
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
    virtual ~ActiveDemoReceiver_msg_()
    {
    }
};


//-------------------------------------
// Main object definition
//-------------------------------------
class ActiveDemoSender: public ActiveObject< ActiveDemoSender_msg_* >
{
  friend class DemoSender;
  DemoSender * self;
    virtual elementResult handle( ActiveDemoSender_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  volatile int tosend;
  DemoReceiver* recv;
  public: elementResult startSending(DemoReceiver* recv, int nr);
  protected: void queue_startSending(DemoReceiver* recv, int nr);
  protected:
    ActiveDemoSender(DemoSender* s, string name):
      ActiveObject< ActiveDemoSender_msg_ * >(name), self(s)
      {
      tosend =  20;
      };
};

class ActiveDemoReceiver: public ActiveObject< ActiveDemoReceiver_msg_* >
{
  friend class DemoReceiver;
  DemoReceiver * self;
    virtual elementResult handle( ActiveDemoReceiver_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  public: elementResult printNumber(int nr);
  protected: void queue_printNumber(int nr);
  protected:
    ActiveDemoReceiver(DemoReceiver* s, string name):
      ActiveObject< ActiveDemoReceiver_msg_ * >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveDemoSender_msg_startSending: public ActiveDemoSender_msg_
{
    DemoReceiver* recv;
    int nr;
  public:
    ActiveDemoSender_msg_startSending(DemoReceiver* recv, int nr) : recv(recv), nr(nr)
      {
      };
    virtual string declaration()
    {
      return "DemoSender::startSending(DemoReceiver* recv, int nr)";
    }
    virtual elementResult run(ActiveDemoSender * ao)
      {
        ENTER_MSG;
        elementResult res = ao->startSending(recv, nr);
        LEAVE_MSG;
        return res;
      };
};

class ActiveDemoReceiver_msg_printNumber: public ActiveDemoReceiver_msg_
{
    int nr;
  public:
    ActiveDemoReceiver_msg_printNumber(int nr) : nr(nr)
      {
      };
    virtual string declaration()
    {
      return "DemoReceiver::printNumber(int nr)";
    }
    virtual elementResult run(ActiveDemoReceiver * ao)
      {
        ENTER_MSG;
        elementResult res = ao->printNumber(nr);
        LEAVE_MSG;
        return res;
      };
};


//-------------------------------------
// Active Object wrapper 
//-------------------------------------
/**
 * Represents the stub that will transform each incoming call (method)
 * to an object subclassed from type ActiveDemoSender_msg_
 * The stub itself has an instance of the true active object.
 */
class DemoSender
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveDemoSender object;
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
    DemoSender(string name="DemoSender"): object(this, name) {};
  public:
    void startSending(DemoReceiver* recv, int nr)
    {
      object.queue_startSending(recv, nr);
    };
  public:
    void donothing();
};

/**
 * Represents the stub that will transform each incoming call (method)
 * to an object subclassed from type ActiveDemoReceiver_msg_
 * The stub itself has an instance of the true active object.
 */
class DemoReceiver
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveDemoReceiver object;
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
    DemoReceiver(string name="DemoReceiver"): object(this, name) {};
  public:
    void printNumber(int nr)
    {
      object.queue_printNumber(nr);
    };
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveDemoSender::queue_startSending(DemoReceiver* recv, int nr)
  {
    push(new ActiveDemoSender_msg_startSending(recv, nr));
  };
inline void ActiveDemoReceiver::queue_printNumber(int nr)
  {
    push(new ActiveDemoReceiver_msg_printNumber(nr));
  };
#endif // __DEMO1_H
