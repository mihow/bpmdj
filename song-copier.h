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

#ifndef __SONG_COPIER_H
#define __SONG_COPIER_H
#include "active-objects.h"
#include "reference-count.h"
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
class ActiveSongCopier_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveSongCopier * /* caller */)
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
class ActiveSongCopier: public ActiveObject<Smart< ActiveSongCopier_msg_ > >
{
  friend class SongCopier;
  SongCopier * self;
    virtual elementResult handle(Smart< ActiveSongCopier_msg_> cmd)
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
      ActiveObject<Smart< ActiveSongCopier_msg_ > >(name), self(s)
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
class SongCopier
{
  private:
    ActiveSongCopier object;
  public:
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
    push(Smart<ActiveSongCopier_msg_setTarget>(
        new ActiveSongCopier_msg_setTarget(trgt)));
  };
inline void ActiveSongCopier::queue_fetch(Song* s)
  {
    push(Smart<ActiveSongCopier_msg_fetch>(
        new ActiveSongCopier_msg_fetch(s)));
  };
inline void ActiveSongCopier::queue_fetchNextSong()
  {
    push(Smart<ActiveSongCopier_msg_fetchNextSong>(
        new ActiveSongCopier_msg_fetchNextSong()));
  };
inline void ActiveSongCopier::queue_terminate()
  {
    push(Smart<ActiveSongCopier_msg_terminate>(
        new ActiveSongCopier_msg_terminate()));
  };
#endif // __SONG_COPIER_H
