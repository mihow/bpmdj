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

#ifndef __CLUSTERER_H
#define __CLUSTERER_H
#include "active-objects.h"
#include "reference-count.h"
using namespace std;
#include "cluster.h"
#include "song-metric.h"
class Clusterer;
class ActiveClusterer;
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
 * the true ActiveClusterer. ActiveClusterer_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveClusterer_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub Clusterer
 * The message class is also an instance of ReferenceCount, which makes it 
 * ideally suited to use within Smart pointers.
 */
class ActiveClusterer_msg_: public ReferenceCount
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveClusterer itself.
     */
    virtual elementResult run(ActiveClusterer * caller)
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
class ActiveClusterer: public ActiveObject<Smart< ActiveClusterer_msg_ > >
{
  friend class Clusterer;
  Clusterer * self;
    virtual elementResult handle(Smart< ActiveClusterer_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  PointSet tocluster;
  public: elementResult reset();
  protected: void queue_reset();
  public: elementResult add(Point* p);
  protected: void queue_add(Point* p);
  Couple* agglomerate_allsteps(Metriek* metriek);
  public: elementResult agglomerate(SongMetriek metriek);
  protected: void queue_agglomerate(SongMetriek metriek);
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveClusterer(Clusterer* s, string name):
      ActiveObject<Smart< ActiveClusterer_msg_ > >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveClusterer_msg_reset: public ActiveClusterer_msg_
{
    ;
  public:
    ActiveClusterer_msg_reset()
      {
      };
    virtual string declaration()
    {
      return "Clusterer::reset()";
    }
    virtual elementResult run(ActiveClusterer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->reset();
        LEAVE_MSG;
        return res;
      };
};

class ActiveClusterer_msg_add: public ActiveClusterer_msg_
{
    Point* p;
  public:
    ActiveClusterer_msg_add(Point* p) : p(p)
      {
      };
    virtual string declaration()
    {
      return "Clusterer::add(Point* p)";
    }
    virtual elementResult run(ActiveClusterer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->add(p);
        LEAVE_MSG;
        return res;
      };
};

class ActiveClusterer_msg_agglomerate: public ActiveClusterer_msg_
{
    SongMetriek metriek;
  public:
    ActiveClusterer_msg_agglomerate(SongMetriek metriek) : metriek(metriek)
      {
      };
    virtual string declaration()
    {
      return "Clusterer::agglomerate(SongMetriek metriek)";
    }
    virtual elementResult run(ActiveClusterer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->agglomerate(metriek);
        LEAVE_MSG;
        return res;
      };
};

class ActiveClusterer_msg_terminate: public ActiveClusterer_msg_
{
    ;
  public:
    ActiveClusterer_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "Clusterer::terminate()";
    }
    virtual elementResult run(ActiveClusterer * ao)
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
 * to an object subclassed from type ActiveClusterer_msg_
 * The stub itself has an instance of the true active object.
 */
class Clusterer
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveClusterer object;
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
    Clusterer(string name="Clusterer"): object(this, name) {};
  public:
    void reset()
    {
      object.queue_reset();
    };
  public:
    void add(Point* p)
    {
      object.queue_add(p);
    };
  public:
    void agglomerate(SongMetriek metriek)
    {
      object.queue_agglomerate(metriek);
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
inline void ActiveClusterer::queue_reset()
  {
    push(Smart<ActiveClusterer_msg_reset>(
        new ActiveClusterer_msg_reset()));
  };
inline void ActiveClusterer::queue_add(Point* p)
  {
    push(Smart<ActiveClusterer_msg_add>(
        new ActiveClusterer_msg_add(p)));
  };
inline void ActiveClusterer::queue_agglomerate(SongMetriek metriek)
  {
    push(Smart<ActiveClusterer_msg_agglomerate>(
        new ActiveClusterer_msg_agglomerate(metriek)));
  };
inline void ActiveClusterer::queue_terminate()
  {
    push(Smart<ActiveClusterer_msg_terminate>(
        new ActiveClusterer_msg_terminate()));
  };
#endif // __CLUSTERER_H
