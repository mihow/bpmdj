/****
 Borg IV
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

#ifndef __BORGIV_ACTIVE_OBJECTS_H__
#define __BORGIV_ACTIVE_OBJECTS_H__
using namespace std;
#line 1 "active-objects.h++"
#include <iostream>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <queue>
#include <deque>
#include <time.h>
#include "ao-scheduler.h"
#include "ao-pool.h"
using namespace std;
typedef enum{RevisitLater, Revisit, RevisitAfterIncoming, Done, Interrupt} elementResult;

/**
 * @defgroup ao Active Objects
 * 
 * These pages document the active objects as used in Borg4. The active objects are supported by a compact
 * runtime that allows messages to be queued into objects, which will then in turn be activated as soon as
 * new messages are available. The runtime is mainly accessed through the @ref ActiveObject class.
 * 
 * There is also a compiler available that will automatically create code for active objects. 
 * The aoc compiler generates a wrapper for an active object. The inside of that object
 * will be the active object that is shielded from the rest of the world. The 
 * outside (the meta-object) will accept all the incoming calls and forward them 
 * to the active object. The following header file, demo1.ao declares two active objects, Sender and Receiver. 
 * The sender will accept startSending. The receiver will accept printNumber. The aoc compiler will automatically
 * generate an appropriate header that will declare two seperate object. In the case of the sender 
 * active object, these will be called Sender and ActiveSender. The ActiveSender will contain 
 * all the methods we declared (except for the meta method). The Sender class will be a proxy 
 * and can be accessed concurrently. All the arguments to each call will be wrapped into a 
 * message and passed to the underlying activeObject. If access to the object fields is necessary
 * then a meta method must be declared.
 *
 * File: demo1.ao; compile with aoc demo1.ao >demo1.h
 *
 *
 *  <code><pre>
 *   active DemoSender
 *     {
 *     int tosend = 20;
 *     DemoReceiver * recv;
 *     message startSending(DemoReceiver* recv, int nr);
 *     meta void donothing();
 *     };
 *
 *   active DemoReceiver
 *     {
 *     message printNumber(int nr);
 *     };
 *  </pre></code>
 *
 * The implementation side of these two active objects looks as follows. File: @ref demo1.cpp
 *
 *  <code><pre>
 *   \#include "demo1.h"
 *   elementResult ActiveDemoSender::startSending(DemoReceiver* recv, int a)
 *     {
 *     tosend = a;
 *     for(int i = 0 ; i < tosend ;  i++)
 *     recv->printNumber(i);
 *     return Done;
 *     }
 *
 *   elementResult ActiveDemoReceiver::printNumber(int nr)
 *     {
 *     printf("%d  ",nr);
 *     fflush(stdout);
 *     return RevisitAfterIncoming;
 *     }
 * 
 *   int main(int, char* [])
 *     {
 *     DemoSender * sender = new DemoSender();
 *     DemoReceiver * recv = new DemoReceiver();
 *     sender->startSending(recv,100);
 *     sleep(100);
 *     }
 *  </pre></code>
 *
 * @todo with respect to the compiler we should allow any block outside the active section.
 * 
 * @todo the compiler should export first all the global vars and then each active object one
 *       by one.
 * 
 * @todo active objects should be able to inherit from each other. If I is an interface and A
 * the active object that must inherit from it, then we get
 * BA: BI and MA: MI
 */

/**
 * @ingroup ao
 * @brief Active Objects perform their task 
 * seperated in time (control flow) and space (data) from the rest of the program.
 *
 * Active objects do not share data between each other and they can only be accessed
 * through message transfer (@ref push). As a simple (but powerful) process model they
 * allow the encapsulation of logical processes into syntactic units. Looking at the code
 * we know automatically which thread will run through which section. Within an active
 * object we can be sure of the following conditions:
 * - when handling a message, we have full read/write access to all the fields
 * - nobody else will be accessing the fields.
 * 
 * When using an active object we know that
 * - sending a message to it will return immediatelly. 
 * - the message will arrive at a later time and will be handled in its
 *   own virtual thread.
 * - messages will arrive in the order they were queued.
 * 
 * An active object can be locked to gain access to its outgoing queues 
 * (if these are present). When an active object is locked, it does not mean 
 * that the object cannot run. It merely means, that the incoming and outgoing 
 * queues are locked. To avoid concurrency problems at this front, the active object
 * will copy the messages that require handling to a sepearte queue before 
 * calling handle.
 */
template <class message>
class ActiveObject: public Lock, protected Runnable
{
 private:
  /**
   * The private queue which accepts incoming messages in the
   * object is locked.
   */
  deque<message> incoming;
 protected:
  /**
   * To make the lockign window as small as possible, we use two queues. 
   * The incoming queue accepts incoming messages and can only be altered when
   * the active object is synchronized. The handling queue can be modified only
   * by the active object and will contain the transferred messages from the
   * incoming queue when handling messages.
   */
  deque<message> handling;
 protected:
  /**
   * All active objects have a name, which can be used during debugging.
   * the scheduler can be filled in, if not, a standard SpawnWhenActivated
   * scheduler will be assigned.
   */
  ActiveObject(string name): Runnable(name)
  {
    active = false;
    changed = false;
    assert(aoPool);
    scheduler = aoPool->sunrise(name);
    assert(scheduler);
  };
public:
  /**
   * Pushes a message to be processed asynchronously.
   * This method can be publicly accessed and is the main access point to send
   * a message to the active object. If the object should start in response to the 
   * message, it will contact the scheduler. This object will acquire the active 
   * object lock, push the message and release the lock.
   */
  void push(message d)
  {
    Synchronized(this);
    push_internal(d);
  }
  /**
   * Pushes a list of messages atomically into the incoming queue. The handling
   * is of course not guaranteed to have the same boundaries as the atomic pushes.
   */
  void push(vector<message> v)
  {
    Synchronized(this);
    for(int i = 0 ; i<v.size();i++)
      push_internal(v[i]);
  }
  /**
   * Deletes the object. This method checks that the object is effectively
   * deactivated. If not, it will throw an error
   */
  virtual ~ActiveObject()
  {
    if (active)
      cerr << "Active Object " << name << " is still activate\n";
    if (scheduler)
      cerr << "Active Object " << name << " can still be activated\n";
  }
private:
  /**
   * This method can be used to push a message, if the caller already acquired the
   * active object lock. This is useful if we want to assure that a collection
   * of messages arrives uninterrupted at the queue. This method will activate
   *  the active object if need be.
   */
  void push_internal(message d)
  {
    assert(locked);
    incoming.push_back(d);
    activate();
  }
 protected:
  /**
   * Describes the scheduler that will be contacted when this
   * object requires activation. If the scheduler is set to 
   * NULL then the active object has been deactivated.
   * Only when an object is deactivated can it be destroyed.
   */
  Scheduler * scheduler;
 public:
  /**
   * returns true when there is no longer a scheduler. This function
   * is used by the scheduler to detect that the object has been terminated. If
   * so the scheduler will notify someone else of this fact.
   */
  virtual bool terminated()
  {
    return scheduler==NULL;
  }
 private:
  /**
   * The lock that allow atomic updating of @ref active and @ref changed
   */
  Lock updating_state;
  /**
   * True when we requested the creation of a thread. This flag should
   * only be update when the @ref updating_state lock has been acquired.
   */
  bool active;
  /**
   * True when we _would_ have asked the creation of a thread, but 
   * we already did so. See activate() for more details on this.
   * This flag should
   * only be update when the @ref updating_state lock has been acquired.
   */
  bool changed;
  /**
   * The activation of the queues is a rather complicated
   * manner. We have two locks involved. The first ist the
   * active object lock. The second one is the @ref updating_state
   * lock. When the control flow enters the activate method, the
   * situation is as follows.
   * - we have only one thread entering. We cannot have multiple concurrent 
   *   activate requests since these can only come from the @ref push_internal
   *   method. That method will keep the active object lock until it returns.
   * - we cannot be sure at this moment that a previous
   *   activate request has not already initiated a thread creation
   *   process.
   * - we can also not be sure that the active thread is not
   *   finishing and will thus not see the freshly pushed message.
   * 
   * To avoid confusion on these matters, we work with a boolean flag
   * (active) that reflects the fact that we started a scheduler, which 
   * has not yet ended. The active flag will be reset by the handler
   * thread if nothing has potentially changed. To know wheter
   * a change could have occured we use the changed flag, which is true
   * if we want to activate the queue but diod not do so since a thread
   * request was already sent out.
   *
   * To summarize: if we want to start a thread we 1st) set the changed 
   * flag and 2nd) if not active, set the active flag and start a 
   * scheduler. When the handler thread wants to stop, it first checks
   * the changed flag, and if it is true, starts another round of message
   * handling. If it is not true, it will switch of the active flag.
   * Of course the checking and setting of these flags need to be set
   * atomically. For that purpose we use the updating_stat lock.
   * We do not use the active object lock since that would interfere
   * with the push call. Effectively: as long as we are pushing, the
   * object could not start, which is an unncessary restriction.
   */
  void activate()
    {
      if (!scheduler) return;
      {
	Synchronized(updating_state);
	changed = true;
	if (active) return;
	active = true;
      }
      assert(scheduler);
      scheduler->start(this);
    }
protected:
  /**
   * Tells the object to no longer start a thread when a message arrives
   * It also signals that the handle() routine should stop its work as 
   * well.
   */
  void deactivate()
  {
    assert(scheduler);
    scheduler = false;
  }
private:
  /**
   * The step method is called by the scheduler to
   * perform one turn of the message handling wheel
   * This function must return true when it should be 
   * called again. This method will a) transfer the 
   * necessary message from the inqueue, b) execute 
   * the handle method, c) check whether something changed
   * and return true if it did (see activate()).
   */
  bool step()
    {
      bool transfer_needed = false;
      {
	Synchronized(updating_state);
	if (changed)
	  {
	    transfer_needed = true;
	    changed = false;
	  }
      }
      if (transfer_needed)
	/**
	 * @internal transfers the incoming messages to the handling queue, preserving the order.
	 * This method locks the active object during its execution.
	 */
	{
	  Synchronized(this);
	  while(!incoming.empty())
	    {
	      handling.push_back(incoming.front());
	      incoming.pop_front();
	    }
	}
      /**
       * @internal We had a logic here that would handle as many message as possible with a 
       * routine such as 'if (handle_as_many_as_possible) while(handle());' but that
       * broke the semantics of the handle routine. Between calls to the handle routine
       * one can assume that the incoming messages have been trasnferred. With the 
       * above statement, we could no longer assume that and needed an artificial fetch_incoming
       * routine.
       */
      if (handle() && scheduler)
	return true;
      {
	Synchronized(updating_state);
	if (changed) return true;
	active = false;
	return false;
      }
    }
protected:
  /**
   * This method is called from within the scheduler. It should
   * perform the runnable action, which in this case it the handling of messages.
   * If handle_as_many_as_possible, 
   * it should perform as many steps as possible before returning. If not,
   * it should return asap. This function should return true if a next call
   * is required, false otherwise.
   * The handle_as_many_as_possible is useful for speed purposes.
   * If called from within a real thread, then there is 
   * no point int returning control once in a while to the 
   * calling thread. On the other hand, if called from 
   * within a virtual thunk based scheduler then it 
   * probably better to return control as often as possible.
   * This function works closely together with the run
   * method, which is the one that should be called
   * from within a scheduler.
   */
  virtual bool run(bool handle_as_many_as_possible)
    {
      if (handle_as_many_as_possible)
	{
	  while(step());
	  return false;
	}
      else
	return step();
    }
  /**
   * This method will be called in its own (virtual) thread, to handle a collection
   * of messages. The messages to be handled are listed in the protected handle field.
   * That queue can be accessed and manipulated as necessary. This method should 
   * return true when it can perform more work or false when it has nothing more to 
   * do (handling empty for instance, or no useful message left in handling).
   * The standard implementation will take the front of the handling queue and 
   * present it to the more specific handle(T) member. Based on the return of that 
   * function a decission is made for the specif handling element.
   */
  virtual bool handle()
    {
      if (handling.empty()) 
	return false;
      message f = handling.front();
      elementResult hr = handle(f);
      switch(hr)
	{
	case RevisitLater:
	  handling.push_back(f);
	case Done:
	  handling.pop_front();
	  break;
	case Revisit:
	  break;
	case Interrupt:
	  return false;
	case RevisitAfterIncoming:
	  push(f);
	  handling.pop_front();
	  break;
	default:
	  assert(0);
	}
      return !handling.empty();
    }
  /**
   * The standard queue handler (handle()), looks at each element in turn
   * for each element it calls handle(T), which  should return one of the following states
   * - RevisitLater: will put the current message at the end of the current handling queue
   * - Revisit: will keep the element at the beginning of the handling queue
   * - RevisitAfterIncoming: will revisit the element after all the new incoming messages
   * - Done: zaps the element from the queue.
   * - Interrupt: stop the handling until a new element arrives
   * 
   * The standard implementation of this method prints an error and crashes.
   */
  virtual elementResult handle(message)
  {
    cerr << name << " lacks a handle(message) routine\n";
    assert(0);
    return Interrupt;
  }
};
#endif
