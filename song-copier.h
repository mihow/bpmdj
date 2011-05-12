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

#ifndef __SONG_COPIER_H
#define __SONG_COPIER_H
#include "./active-objects.h"
using namespace std;
#include "song.h"
#include "active-objects.h"
class SongCopier;
class ActiveSongCopier;
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
 * the true ActiveSongCopier. ActiveSongCopier_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveSongCopier_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub SongCopier
 */
class ActiveSongCopier_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveSongCopier itself.
     */
    virtual elementResult run(ActiveSongCopier * /* caller */)
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
class ActiveSongCopier: public ActiveObject< ActiveSongCopier_msg_* >
{
  friend class SongCopier;
  SongCopier * self;
    virtual elementResult handle( ActiveSongCopier_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  deque < Song* >  tocopy;
  QString target;
  public: elementResult setTarget(QString trgt);
  protected: void queue_setTarget(QString trgt);
  public: elementResult fetch(Song* s);
  protected: void queue_fetch(Song* s);
  public: elementResult fetchNextSong();
  protected: void queue_fetchNextSong();
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveSongCopier(SongCopier* s, string name):
      ActiveObject< ActiveSongCopier_msg_ * >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveSongCopier_msg_setTarget: public ActiveSongCopier_msg_
{
    QString trgt;
  public:
    ActiveSongCopier_msg_setTarget(QString trgt) : trgt(trgt)
      {
      };
    virtual string declaration()
    {
      return "SongCopier::setTarget(QString trgt)";
    }
    virtual elementResult run(ActiveSongCopier * ao)
      {
        ENTER_MSG;
        elementResult res = ao->setTarget(trgt);
        LEAVE_MSG;
        return res;
      };
};

class ActiveSongCopier_msg_fetch: public ActiveSongCopier_msg_
{
    Song* s;
  public:
    ActiveSongCopier_msg_fetch(Song* s) : s(s)
      {
      };
    virtual string declaration()
    {
      return "SongCopier::fetch(Song* s)";
    }
    virtual elementResult run(ActiveSongCopier * ao)
      {
        ENTER_MSG;
        elementResult res = ao->fetch(s);
        LEAVE_MSG;
        return res;
      };
};

class ActiveSongCopier_msg_fetchNextSong: public ActiveSongCopier_msg_
{
    ;
  public:
    ActiveSongCopier_msg_fetchNextSong()
      {
      };
    virtual string declaration()
    {
      return "SongCopier::fetchNextSong()";
    }
    virtual elementResult run(ActiveSongCopier * ao)
      {
        ENTER_MSG;
        elementResult res = ao->fetchNextSong();
        LEAVE_MSG;
        return res;
      };
};

class ActiveSongCopier_msg_terminate: public ActiveSongCopier_msg_
{
    ;
  public:
    ActiveSongCopier_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "SongCopier::terminate()";
    }
    virtual elementResult run(ActiveSongCopier * ao)
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
 * to an object subclassed from type ActiveSongCopier_msg_
 * The stub itself has an instance of the true active object.
 */
class SongCopier
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveSongCopier object;
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
    SongCopier(string name="SongCopier"): object(this, name) {};
  public:
    void setTarget(QString trgt)
    {
      object.queue_setTarget(trgt);
    };
  public:
    void fetch(Song* s)
    {
      object.queue_fetch(s);
    };
  public:
    void fetchNextSong()
    {
      object.queue_fetchNextSong();
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
inline void ActiveSongCopier::queue_setTarget(QString trgt)
  {
    push(new ActiveSongCopier_msg_setTarget(trgt));
  };
inline void ActiveSongCopier::queue_fetch(Song* s)
  {
    push(new ActiveSongCopier_msg_fetch(s));
  };
inline void ActiveSongCopier::queue_fetchNextSong()
  {
    push(new ActiveSongCopier_msg_fetchNextSong());
  };
inline void ActiveSongCopier::queue_terminate()
  {
    push(new ActiveSongCopier_msg_terminate());
  };
#endif // __SONG_COPIER_H
