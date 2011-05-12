/****
 Active Object compiled file
 Copyright (C) 2006-2009 Werner Van Belle
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

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
class ActiveClusterer_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveClusterer * /* caller */)
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
class ActiveClusterer: public ActiveObject<Smart< ActiveClusterer_msg_ > >
{
  friend class Clusterer;
  Clusterer * self;
    virtual elementResult handle(Smart< ActiveClusterer_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  Cluster cluster;
  public: elementResult reset();
  protected: void queue_reset();
  public: elementResult addPoint(Point* p);
  protected: void queue_addPoint(Point* p);
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

class ActiveClusterer_msg_addPoint: public ActiveClusterer_msg_
{
    Point* p;
  public:
    ActiveClusterer_msg_addPoint(Point* p) : p(p)
      {
      };
    virtual string declaration()
    {
      return "Clusterer::addPoint(Point* p)";
    }
    virtual elementResult run(ActiveClusterer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->addPoint(p);
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
class Clusterer
{
  private:
    ActiveClusterer object;
  public:
    Clusterer(string name="Clusterer"): object(this, name) {};
  public:
    void reset()
    {
      object.queue_reset();
    };
  public:
    void addPoint(Point* p)
    {
      object.queue_addPoint(p);
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
inline void ActiveClusterer::queue_addPoint(Point* p)
  {
    push(Smart<ActiveClusterer_msg_addPoint>(
        new ActiveClusterer_msg_addPoint(p)));
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
