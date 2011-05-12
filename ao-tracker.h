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

#ifndef __AO_TRACKER_H
#define __AO_TRACKER_H
#include "active-objects.h"
#include "reference-count.h"
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
class ActiveAoTracker_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveAoTracker * /* caller */)
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
class ActiveAoTracker: public ActiveObject<Smart< ActiveAoTracker_msg_ > >
{
  friend class AoTracker;
  AoTracker * self;
    virtual elementResult handle(Smart< ActiveAoTracker_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  set < string >  alive;
  public: elementResult sunset(string s);
  protected: void queue_sunset(string s);
  public: elementResult sunrise(string s);
  protected: void queue_sunrise(string s);
  protected:
    ActiveAoTracker(AoTracker* s, string name):
      ActiveObject<Smart< ActiveAoTracker_msg_ > >(name), self(s)
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


//-------------------------------------
// Active Object wrapper 
//-------------------------------------
class AoTracker
{
  private:
    ActiveAoTracker object;
  public:
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
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveAoTracker::queue_sunset(string s)
  {
    push(Smart<ActiveAoTracker_msg_sunset>(
        new ActiveAoTracker_msg_sunset(s)));
  };
inline void ActiveAoTracker::queue_sunrise(string s)
  {
    push(Smart<ActiveAoTracker_msg_sunrise>(
        new ActiveAoTracker_msg_sunrise(s)));
  };
#endif // __AO_TRACKER_H
