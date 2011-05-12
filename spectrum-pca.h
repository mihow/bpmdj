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

#ifndef __SPECTRUM_PCA_H
#define __SPECTRUM_PCA_H
#include "./active-objects.h"
using namespace std;
#include <vector>
#include "selector.h"
class SpectrumPca;
class ActiveSpectrumPca;
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
 * the true ActiveSpectrumPca. ActiveSpectrumPca_msg_ has a run method which is invoked
 * by ActiveObject whenever it wants to handle the message. For each 
 * declared method in the active object description, a specific subclass
 * of ActiveSpectrumPca_msg_ has been generated. See inheritance diagram.
 * The message classes are automatically instantiated by the active object
 * stub SpectrumPca
 */
class ActiveSpectrumPca_msg_
{
  public:
    /**
     * Called by ActiveObject to handle this queued message.
     * %arg caller is the ActiveSpectrumPca itself.
     */
    virtual elementResult run(ActiveSpectrumPca * /* caller */)
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
class ActiveSpectrumPca: public ActiveObject< ActiveSpectrumPca_msg_* >
{
  friend class SpectrumPca;
  SpectrumPca * self;
    virtual elementResult handle( ActiveSpectrumPca_msg_* cmd)
      {
        if (cmd) return cmd->run(this);
        else return Done;
      };
  public: elementResult pcaThis(vector < Song* > * songs);
  protected: void queue_pcaThis(vector < Song* > * songs);
  public: elementResult terminate();
  protected: void queue_terminate();
  protected:
    ActiveSpectrumPca(SpectrumPca* s, string name):
      ActiveObject< ActiveSpectrumPca_msg_ * >(name), self(s)
      {
      };
};


//-------------------------------------
// Specific messaging classes 
//-------------------------------------
class ActiveSpectrumPca_msg_pcaThis: public ActiveSpectrumPca_msg_
{
    vector < Song* > * songs;
  public:
    ActiveSpectrumPca_msg_pcaThis(vector < Song* > * songs) : songs(songs)
      {
      };
    virtual string declaration()
    {
      return "SpectrumPca::pcaThis(vector < Song* > * songs)";
    }
    virtual elementResult run(ActiveSpectrumPca * ao)
      {
        ENTER_MSG;
        elementResult res = ao->pcaThis(songs);
        LEAVE_MSG;
        return res;
      };
};

class ActiveSpectrumPca_msg_terminate: public ActiveSpectrumPca_msg_
{
    ;
  public:
    ActiveSpectrumPca_msg_terminate()
      {
      };
    virtual string declaration()
    {
      return "SpectrumPca::terminate()";
    }
    virtual elementResult run(ActiveSpectrumPca * ao)
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
 * to an object subclassed from type ActiveSpectrumPca_msg_
 * The stub itself has an instance of the true active object.
 */
class SpectrumPca
{
  private:
    /**
     * The object that is covered for by this stub. The fact that the
     * stub allocates the object ensures that only one of it exists and that any
     * interaction must go through the stub. As such it is kept private.
     */
    ActiveSpectrumPca object;
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
    SpectrumPca(string name="SpectrumPca"): object(this, name) {};
  public:
    void pcaThis(vector < Song* > * songs)
    {
      object.queue_pcaThis(songs);
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
inline void ActiveSpectrumPca::queue_pcaThis(vector < Song* > * songs)
  {
    push(new ActiveSpectrumPca_msg_pcaThis(songs));
  };
inline void ActiveSpectrumPca::queue_terminate()
  {
    push(new ActiveSpectrumPca_msg_terminate());
  };
#endif // __SPECTRUM_PCA_H
