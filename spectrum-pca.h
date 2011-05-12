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

#ifndef __SPECTRUM_PCA_H
#define __SPECTRUM_PCA_H
#include "active-objects.h"
#include "reference-count.h"
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
class ActiveSpectrumPca_msg_: public ReferenceCount
{
  public:
    virtual elementResult run(ActiveSpectrumPca * /* caller */)
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
class ActiveSpectrumPca: public ActiveObject<Smart< ActiveSpectrumPca_msg_ > >
{
  friend class SpectrumPca;
  SpectrumPca * self;
    virtual elementResult handle(Smart< ActiveSpectrumPca_msg_> cmd)
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
      ActiveObject<Smart< ActiveSpectrumPca_msg_ > >(name), self(s)
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
class SpectrumPca
{
  private:
    ActiveSpectrumPca object;
  public:
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
    push(Smart<ActiveSpectrumPca_msg_pcaThis>(
        new ActiveSpectrumPca_msg_pcaThis(songs)));
  };
inline void ActiveSpectrumPca::queue_terminate()
  {
    push(Smart<ActiveSpectrumPca_msg_terminate>(
        new ActiveSpectrumPca_msg_terminate()));
  };
#endif // __SPECTRUM_PCA_H
