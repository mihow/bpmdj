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

#ifndef __ANALYZER_WORKER_H
#define __ANALYZER_WORKER_H
#include "./Active/active-objects.h"
using namespace std;
#include <vector>
#include "analyzer-impl.h"
class AnalyzerWorker;
class ActiveAnalyzerWorker;
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
 * the true ActiveAnalyzerWorker. ActiveAnalyzerWorker_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveAnalyzerWorker_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub AnalyzerWorker
 */
class ActiveAnalyzerWorker_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveAnalyzerWorker itself.
     */
    virtual elementResult run(ActiveAnalyzerWorker * /* caller */)
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
    virtual ~ActiveAnalyzerWorker_msg_()
    {
    }
};


//-------------------------------------
// Main object definition
//-------------------------------------
class ActiveAnalyzerWorker: public ActiveObject< ActiveAnalyzerWorker_msg_* >
{
  friend class AnalyzerWorker;
  AnalyzerWorker * self;
    virtual elementResult handle( ActiveAnalyzerWorker_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  AnalyzerAlg running;
  public: elementResult run(AnalyzerAlg a);
  protected: void queue_run(AnalyzerAlg a);
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveAnalyzerWorker(AnalyzerWorker* s, string name):
      ActiveObject< ActiveAnalyzerWorker_msg_ * >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveAnalyzerWorker_msg_run: public ActiveAnalyzerWorker_msg_
{
    AnalyzerAlg a;
  public:
    ActiveAnalyzerWorker_msg_run(AnalyzerAlg a) : a(a)
      {
      };
    virtual string declaration()
    {
      return "AnalyzerWorker::run(AnalyzerAlg a)";
    }
    virtual elementResult run(ActiveAnalyzerWorker * ao)
      {
        ENTER_MSG;
        elementResult res = ao->run(a);
        LEAVE_MSG;
        return res;
      };
};

class ActiveAnalyzerWorker_msg_terminate: public ActiveAnalyzerWorker_msg_
{
    ;
  public:
    ActiveAnalyzerWorker_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "AnalyzerWorker::terminate()";
    }
    virtual elementResult run(ActiveAnalyzerWorker * ao)
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
 * to an object subclassed from type ActiveAnalyzerWorker_msg_
 * The stub itself has an instance of the true active object.
 */
class AnalyzerWorker
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveAnalyzerWorker object;
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
    AnalyzerWorker(string name="AnalyzerWorker"): object(this, name) {};
  public:
    void run(AnalyzerAlg a)
    {
      object.queue_run(a);
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
inline void ActiveAnalyzerWorker::queue_run(AnalyzerAlg a)
  {
    push(new ActiveAnalyzerWorker_msg_run(a));
  };
inline void ActiveAnalyzerWorker::queue_terminate()
  {
    push(new ActiveAnalyzerWorker_msg_terminate());
  };
#endif // __ANALYZER_WORKER_H
