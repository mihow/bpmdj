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

#ifndef __AO_SOM_BEATGRAPH_H
#define __AO_SOM_BEATGRAPH_H
#include "active-objects.h"
#include "reference-count.h"
using namespace std;
#include "types.h"
class SomBeatGraph;
class ActiveSomBeatGraph;
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
 * the true ActiveSomBeatGraph. ActiveSomBeatGraph_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveSomBeatGraph_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub SomBeatGraph
 * The message class is also an instance of ReferenceCount, which makes it 
 * ideally suited to use within Smart pointers.
 */
class ActiveSomBeatGraph_msg_: public ReferenceCount
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveSomBeatGraph itself.
     */
    virtual elementResult run(ActiveSomBeatGraph * caller)
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
class ActiveSomBeatGraph: public ActiveObject<Smart< ActiveSomBeatGraph_msg_ > >
{
  friend class SomBeatGraph;
  SomBeatGraph * self;
    virtual elementResult handle(Smart< ActiveSomBeatGraph_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  unsigned4 dimensions;
  unsigned4 vectors;
  unsigned2 colors;
  signed1** mapping;
  signed4** newmapping;
  unsigned2* newmappingcount;
  unsigned4* newmappingstrength;
  signed1** points;
  unsigned1* pointcolors;
  public: elementResult start();
  protected: void queue_start();
  protected:
    ActiveSomBeatGraph(SomBeatGraph* s, string name):
      ActiveObject<Smart< ActiveSomBeatGraph_msg_ > >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveSomBeatGraph_msg_start: public ActiveSomBeatGraph_msg_
{
    ;
  public:
    ActiveSomBeatGraph_msg_start()
      {
      };
    virtual string declaration()
    {
      return "SomBeatGraph::start()";
    }
    virtual elementResult run(ActiveSomBeatGraph * ao)
      {
        ENTER_MSG;
        elementResult res = ao->start();
        LEAVE_MSG;
        return res;
      };
};


//-------------------------------------
// Active Object wrapper 
//-------------------------------------
/**
 * Represents the stub that will transform each incoming call (method)
 * to an object subclassed from type ActiveSomBeatGraph_msg_
 * The stub itself has an instance of the true active object.
 */
class SomBeatGraph
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveSomBeatGraph object;
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
    SomBeatGraph(string name="SomBeatGraph"): object(this, name) {};
  public:
    void start()
    {
      object.queue_start();
    };
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveSomBeatGraph::queue_start()
  {
    push(Smart<ActiveSomBeatGraph_msg_start>(
        new ActiveSomBeatGraph_msg_start()));
  };
#endif // __AO_SOM_BEATGRAPH_H
