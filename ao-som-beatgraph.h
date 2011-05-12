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
class ActiveSomBeatGraph_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveSomBeatGraph * /* caller */)
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
class SomBeatGraph
{
  private:
    ActiveSomBeatGraph object;
  public:
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
