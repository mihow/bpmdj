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

#ifndef __INDEX_READER_H
#define __INDEX_READER_H
#include "active-objects.h"
#include "reference-count.h"
using namespace std;
class IndexScanner;
class IndexReader;
class ActiveIndexReader;
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
class ActiveIndexReader_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveIndexReader * /* caller */)
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
class ActiveIndexReader: public ActiveObject<Smart< ActiveIndexReader_msg_ > >
{
  friend class IndexReader;
  IndexReader * self;
    virtual elementResult handle(Smart< ActiveIndexReader_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  IndexScanner* reader;
  int total_files;
  int expected_files;
  public: elementResult start(int expected);
  protected: void queue_start(int expected);
  public: elementResult thunk();
  protected: void queue_thunk();
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveIndexReader(IndexReader* s, string name):
      ActiveObject<Smart< ActiveIndexReader_msg_ > >(name), self(s)
      {
      expected_files =  1;
      reader = NULL;
      total_files =  0;
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveIndexReader_msg_start: public ActiveIndexReader_msg_
{
    int expected;
  public:
    ActiveIndexReader_msg_start(int expected) : expected(expected)
      {
      };
    virtual string declaration()
    {
      return "IndexReader::start(int expected)";
    }
    virtual elementResult run(ActiveIndexReader * ao)
      {
        ENTER_MSG;
        elementResult res = ao->start(expected);
        LEAVE_MSG;
        return res;
      };
};

class ActiveIndexReader_msg_thunk: public ActiveIndexReader_msg_
{
    ;
  public:
    ActiveIndexReader_msg_thunk()
      {
      };
    virtual string declaration()
    {
      return "IndexReader::thunk()";
    }
    virtual elementResult run(ActiveIndexReader * ao)
      {
        ENTER_MSG;
        elementResult res = ao->thunk();
        LEAVE_MSG;
        return res;
      };
};

class ActiveIndexReader_msg_terminate: public ActiveIndexReader_msg_
{
    ;
  public:
    ActiveIndexReader_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "IndexReader::terminate()";
    }
    virtual elementResult run(ActiveIndexReader * ao)
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
class IndexReader
{
  private:
    ActiveIndexReader object;
  public:
    IndexReader(string name="IndexReader"): object(this, name) {};
  public:
    void start(int expected)
    {
      object.queue_start(expected);
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
inline void ActiveIndexReader::queue_start(int expected)
  {
    push(Smart<ActiveIndexReader_msg_start>(
        new ActiveIndexReader_msg_start(expected)));
  };
inline void ActiveIndexReader::queue_thunk()
  {
    push(Smart<ActiveIndexReader_msg_thunk>(
        new ActiveIndexReader_msg_thunk()));
  };
inline void ActiveIndexReader::queue_terminate()
  {
    push(Smart<ActiveIndexReader_msg_terminate>(
        new ActiveIndexReader_msg_terminate()));
  };
#endif // __INDEX_READER_H
