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

#ifndef __FRAGMENT_PLAYER_H
#define __FRAGMENT_PLAYER_H
#include "./active-objects.h"
using namespace std;
#include "song.h"
#include "active-objects.h"
#include "player-config.h"
#include "dsp-drivers.h"
#include "do-fragment.h"
#include "fragment-deliverer.h"
class FragmentPlayer;
class ActiveFragmentPlayer;
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
 * the true ActiveFragmentPlayer. ActiveFragmentPlayer_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveFragmentPlayer_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub FragmentPlayer
 */
class ActiveFragmentPlayer_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveFragmentPlayer itself.
     */
    virtual elementResult run(ActiveFragmentPlayer * /* caller */)
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
class ActiveFragmentPlayer: public ActiveObject< ActiveFragmentPlayer_msg_* >
{
  friend class FragmentPlayer;
  FragmentPlayer * self;
    virtual elementResult handle( ActiveFragmentPlayer_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  FragmentDeliverer delivery;
  bool finished;
  int player_slot;
  volatile bool stopped;
  public: elementResult playWave(FragmentInMemory fragment);
  protected: void queue_playWave(FragmentInMemory fragment);
  public: elementResult delivererFinished();
  protected: void queue_delivererFinished();
  public: elementResult stopOutput();
  protected: void queue_stopOutput();
  public: elementResult startOutput();
  protected: void queue_startOutput();
  void checkValidDsp();
  void closeDsp();
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveFragmentPlayer(FragmentPlayer* s, string name):
      ActiveObject< ActiveFragmentPlayer_msg_ * >(name), self(s)
      {
      finished = false;
      player_slot = -1;
      stopped = false;
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveFragmentPlayer_msg_playWave: public ActiveFragmentPlayer_msg_
{
    FragmentInMemory fragment;
  public:
    ActiveFragmentPlayer_msg_playWave(FragmentInMemory fragment) : fragment(fragment)
      {
      };
    virtual string declaration()
    {
      return "FragmentPlayer::playWave(FragmentInMemory fragment)";
    }
    virtual elementResult run(ActiveFragmentPlayer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->playWave(fragment);
        LEAVE_MSG;
        return res;
      };
};

class ActiveFragmentPlayer_msg_delivererFinished: public ActiveFragmentPlayer_msg_
{
    ;
  public:
    ActiveFragmentPlayer_msg_delivererFinished()
      {
      };
    virtual string declaration()
    {
      return "FragmentPlayer::delivererFinished()";
    }
    virtual elementResult run(ActiveFragmentPlayer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->delivererFinished();
        LEAVE_MSG;
        return res;
      };
};

class ActiveFragmentPlayer_msg_stopOutput: public ActiveFragmentPlayer_msg_
{
    ;
  public:
    ActiveFragmentPlayer_msg_stopOutput()
      {
      };
    virtual string declaration()
    {
      return "FragmentPlayer::stopOutput()";
    }
    virtual elementResult run(ActiveFragmentPlayer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->stopOutput();
        LEAVE_MSG;
        return res;
      };
};

class ActiveFragmentPlayer_msg_startOutput: public ActiveFragmentPlayer_msg_
{
    ;
  public:
    ActiveFragmentPlayer_msg_startOutput()
      {
      };
    virtual string declaration()
    {
      return "FragmentPlayer::startOutput()";
    }
    virtual elementResult run(ActiveFragmentPlayer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->startOutput();
        LEAVE_MSG;
        return res;
      };
};

class ActiveFragmentPlayer_msg_terminate: public ActiveFragmentPlayer_msg_
{
    ;
  public:
    ActiveFragmentPlayer_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "FragmentPlayer::terminate()";
    }
    virtual elementResult run(ActiveFragmentPlayer * ao)
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
 * to an object subclassed from type ActiveFragmentPlayer_msg_
 * The stub itself has an instance of the true active object.
 */
class FragmentPlayer
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveFragmentPlayer object;
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
    FragmentPlayer(string name="FragmentPlayer"): object(this, name) {};
  public:
    void playWave(FragmentInMemory fragment)
    {
      object.queue_playWave(fragment);
    };
  public:
    void delivererFinished()
    {
      object.queue_delivererFinished();
    };
  public:
    void stopOutput()
    {
      object.queue_stopOutput();
    };
  public:
    void startOutput()
    {
      object.queue_startOutput();
    };
  public:
    void waitForStop();
  public:
    void waitForStart();
  public:
    void terminate()
    {
      object.queue_terminate();
    };
};


//-------------------------------------
// Active Object Methods
//-------------------------------------
inline void ActiveFragmentPlayer::queue_playWave(FragmentInMemory fragment)
  {
    push(new ActiveFragmentPlayer_msg_playWave(fragment));
  };
inline void ActiveFragmentPlayer::queue_delivererFinished()
  {
    push(new ActiveFragmentPlayer_msg_delivererFinished());
  };
inline void ActiveFragmentPlayer::queue_stopOutput()
  {
    push(new ActiveFragmentPlayer_msg_stopOutput());
  };
inline void ActiveFragmentPlayer::queue_startOutput()
  {
    push(new ActiveFragmentPlayer_msg_startOutput());
  };
inline void ActiveFragmentPlayer::queue_terminate()
  {
    push(new ActiveFragmentPlayer_msg_terminate());
  };
#endif // __FRAGMENT_PLAYER_H
