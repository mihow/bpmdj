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

#ifndef __FRAGMENT_PLAYER_H
#define __FRAGMENT_PLAYER_H
#include "active-objects.h"
#include "reference-count.h"
using namespace std;
#include "song.h"
#include "active-objects.h"
#include "player-config.h"
#include "dsp-drivers.h"
#include "do-fragment.h"
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
class ActiveFragmentPlayer_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveFragmentPlayer * /* caller */)
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
class ActiveFragmentPlayer: public ActiveObject<Smart< ActiveFragmentPlayer_msg_ > >
{
  friend class FragmentPlayer;
  FragmentPlayer * self;
    virtual elementResult handle(Smart< ActiveFragmentPlayer_msg_> cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  FragmentInMemory playing;
  unsigned4 curpos;
  bool finished;
  volatile dsp_driver_ptr dsp;
  int player_slot;
  volatile bool stopped;
  public: elementResult playWave(FragmentInMemory fragment);
  protected: void queue_playWave(FragmentInMemory fragment);
  public: elementResult stopOutput();
  protected: void queue_stopOutput();
  public: elementResult startOutput();
  protected: void queue_startOutput();
  void checkValidDsp();
  void closeDsp();
  int expected_playchunk;
  public: elementResult playChunk(int t);
  protected: void queue_playChunk(int t);
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveFragmentPlayer(FragmentPlayer* s, string name):
      ActiveObject<Smart< ActiveFragmentPlayer_msg_ > >(name), self(s)
      {
      curpos =  0;
      dsp = NULL;
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

class ActiveFragmentPlayer_msg_playChunk: public ActiveFragmentPlayer_msg_
{
    int t;
  public:
    ActiveFragmentPlayer_msg_playChunk(int t) : t(t)
      {
      };
    virtual string declaration()
    {
      return "FragmentPlayer::playChunk(int t)";
    }
    virtual elementResult run(ActiveFragmentPlayer * ao)
      {
        ENTER_MSG;
        elementResult res = ao->playChunk(t);
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
class FragmentPlayer
{
  private:
    ActiveFragmentPlayer object;
  public:
    FragmentPlayer(string name="FragmentPlayer"): object(this, name) {};
  public:
    void playWave(FragmentInMemory fragment)
    {
      object.queue_playWave(fragment);
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
    void playChunk(int t)
    {
      object.queue_playChunk(t);
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
    push(Smart<ActiveFragmentPlayer_msg_playWave>(
        new ActiveFragmentPlayer_msg_playWave(fragment)));
  };
inline void ActiveFragmentPlayer::queue_stopOutput()
  {
    push(Smart<ActiveFragmentPlayer_msg_stopOutput>(
        new ActiveFragmentPlayer_msg_stopOutput()));
  };
inline void ActiveFragmentPlayer::queue_startOutput()
  {
    push(Smart<ActiveFragmentPlayer_msg_startOutput>(
        new ActiveFragmentPlayer_msg_startOutput()));
  };
inline void ActiveFragmentPlayer::queue_playChunk(int t)
  {
    push(Smart<ActiveFragmentPlayer_msg_playChunk>(
        new ActiveFragmentPlayer_msg_playChunk(t)));
  };
inline void ActiveFragmentPlayer::queue_terminate()
  {
    push(Smart<ActiveFragmentPlayer_msg_terminate>(
        new ActiveFragmentPlayer_msg_terminate()));
  };
#endif // __FRAGMENT_PLAYER_H
