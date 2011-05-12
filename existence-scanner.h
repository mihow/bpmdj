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

#ifndef __EXISTENCE_SCANNER_H
#define __EXISTENCE_SCANNER_H
#include "active-objects.h"
#include "reference-count.h"
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
class ActiveExistenceScanner_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveExistenceScanner * /* caller */)
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
class ActiveExistenceScanner: public ActiveObject<Smart< ActiveExistenceScanner_msg_ > >
{
  friend class ExistenceScanner;
    virtual elementResult handle(Smart< ActiveExistenceScanner_msg_> cmd)
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
    ActiveExistenceScanner(string name="ExistenceScanner"):
      ActiveObject<Smart< ActiveExistenceScanner_msg_ > >(name)
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
class ExistenceScanner
{
  private:
    ActiveExistenceScanner object;
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
    push(Smart<ActiveExistenceScanner_msg_start>(
        new ActiveExistenceScanner_msg_start(all)));
  };
inline void ActiveExistenceScanner::queue_thunk()
  {
    push(Smart<ActiveExistenceScanner_msg_thunk>(
        new ActiveExistenceScanner_msg_thunk()));
  };
inline void ActiveExistenceScanner::queue_terminate()
  {
    push(Smart<ActiveExistenceScanner_msg_terminate>(
        new ActiveExistenceScanner_msg_terminate()));
  };
#endif // __EXISTENCE_SCANNER_H
