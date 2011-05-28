/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__dsp_drivers_h__
#define __loaded__dsp_drivers_h__
using namespace std;
#line 1 "dsp-drivers.h++"
#include "common.h"
#include "stereo-sample2.h"
#include "player-config.h"

#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#ifndef COMPILE_JACK
#error -------------------------------------------
#error Should at least compile one dsp driver !!!
#error Use Try -D COMPILE_OSS, -D COMPILE_ALSA or
#error -D COMPILE_JACK
#error -------------------------------------------
#endif
#endif
#endif

class PlayerConfig;

/**
 * A class which can deliver data to the dsp_driver
 * when requested. The read method should for each 
 * invocation always return the next sample to be 
 * played. The audio_source is passed to the dsp_driver
 * which should use it to obtain the audio to be played
 * read should be invoked pseudo-realtime. This class
 * should not be used to buffer 3 minutes of music before
 * playing.
 */
class audio_source
{
public:
  /**
   * The read method should return the next sample to be played.
   * It should never wait. If no data is available it can _signal_ to 
   * some other thread that this is the case, but it should not wait
   * for data to become available. Similar, it should not wait if 
   * a pause request has been issued. In those cases it is safe
   * to return 0.
   */
  virtual stereo_sample2 read()=0;
};

/**
 * BpmDj can use any sound device driver necessary given that a class exists to
 * support such a driver. For instance, we currently have an ALSA driver, an 
 * old fashioned OSS driver and an experimental Jack driver. Examples of 
 * drivers can be found in dsp-alsa.h and dsp-oss.h.
 *
 * The dsp driver is responsible for having its own thread of control, created
 * when the start() method is called. The standard dsp_driver will generate a
 * thread that will take samples from the input and place them into the write 
 * thread.
 */
class dsp_driver
{
protected:
  bool verbose;
  /**
   * Set to true when an external thread requests a stop of the driver, false
   * otherwise.
   */
  volatile bool stop_request;
  /**
   * Set to true when the thread stopped. This flag is only used in the
   * stop operation to perform a wait.
   */
  volatile bool stopped;
  /**
   * The audio source. This one is set when invoking the start method.
   */
  audio_source* audio;
public:
  /**
   * The playrate for this dsp driver. E.g: 44100 or 48000
   */
  unsigned4 playrate;
  /**
   * A method which will performs a continuous data pull from the core and push
   * it to the driver. This function should be called by nobody but the go2 
   * function in dsp-drivers.cpp
   */
  void run_pusher_thread();
public:
  // playing
  dsp_driver(const PlayerConfig & config);
  /**
   * The normal start routine will spawn a process which will 
   * rely on a synchronous writing of data to the output. The start
   * routine must return directly after the thread was created
   * and the pause flag is set correctly.
   */
  virtual void start(audio_source *) = 0;
  /**
   * The stop method is called from an external thread. It should signal 
   * the driver that any playing thread should stop and the dsp device closed
   * once  done. When all this is finished, the method should return.
   * This should be a synchronous call.
   */
  virtual void stop(); 
public:
  /**
   * The pause function is called from within the player to signal that the
   * dsp driver should stop playback immediately, in such a manner that it can
   * later continue directly. The pause() function just signals the request 
   * and should return directly. It is the internal_pause which should perform
   * the appropriate dsp alterations to actually stop the playback.
   * 
   * In case of the internal synchronous driver (the one which relies on the 
   * run_pusher_thread, the control flow consists of the following threads and
   * steps.
   * - an external thread (the user interface thread for instance) will call
   *   the pause() function. This function will set the paused flag to true 
   *   and return.
   * - the transfer thread running in run_pusher_thread will detect this flag 
   *   and call the internal_pause() routine which should actually stop the dsp
   *   playback
   * - the transfer thread will then wait until the pause flag is again set to
   *   false
   * - the user interface thread calls the unpause() method which sets the 
   *   pause flag to false;
   * - the transfer thread will then call internal_unpause to re-enable the dsp
   *   playback
   * - the transfer thread will then continue playing.
   * 
   * When writing a dsp driver it is unlikely that you need to override the 
   * pause function. It is more likely that you want to override the 
   * internal_pause() and internal_unpause() methods.
   */ 
  virtual void pause();
  /**
   * The unpause method signals that playback should continue. The function 
   * should return directly. It is up to the internal_unpause to alter the dsp
   * configuration. See pause() for more information.
   */
  virtual void unpause();
  /**
   * Returns the current pause state. true if playback is temporarily halted. 
   * False otherwise.
   */
  bool get_paused() 
  { 
    return paused; 
  }
protected: 
  /**
   * The internal pause method is the one responsible for altering the dsp 
   * configuration. This function should return as well. After pause_internal, 
   * and internal_unpause will be called to signal the end of the pause.
   * See pause() for more information.
   */
  virtual void internal_pause()=0; 
  virtual void internal_unpause()=0;
  /**
   * The paused field is set to true when no sound should be produced. To 
   * pause the output of a dsp driver call the pause() function. To unpause 
   * the playback call the unpause function. When using the standard threaded
   * pusher one can/should rely on the wait_for_unpause function to continue 
   * playing back.
   */
  volatile bool paused;
  volatile bool starting;
private:  
  /**
   * If the player was pause, will wait until some other thread sets the 
   * paused flag to false again.
   */
  void wait_for_unpause();
public:
  /**
   * The write function should write out the specified value and block when
   * necessary.It is mainly used by the pusher thread at the moment. If you plan
   * to write a complete asynchronous driver you might not need to implement 
   * this function.
   */
  virtual void write(stereo_sample2 value) = 0;
  /**
   * This method must return the latency between the sample position that
   * currently comes out of the speakers and the next sample that will be
   * read from the audio source.
   */
  virtual signed8 latency() = 0;
  /**
   * The open function should initialize the specific device such that we can
   * start writing to it and that it will play sound. If the open is not 
   * possible this function should return err_dsp. If the function is 
   * successful open() should return err_none. If user interface is set to true
   * then error dialogs will be shown when errors appear. If the open fails
   * and returns err_dsp, then expect a close to be followed afterward.
   */
  virtual int  open(bool ui) = 0;
  virtual void close(bool flush_first) = 0;
  /**
   * Will do nothing but delete the object. The driver should make sure
   * that any callbacks to this object will no longer occur. Because after the 
   * destructor is called the object is no longer in existence and any callback
   * might segfault as a result.
   */
  virtual ~dsp_driver() 
  {
  };
  /**
   * This method is called once by each started application.
   */
  static void init();
  static dsp_driver * get_driver(PlayerConfig * cfg);
  bool get_stopped() { return stopped; }
};

/**
 * This type is used to pass to the active object 'FragmentPlayer'
 */
typedef dsp_driver* dsp_driver_ptr;
extern dsp_driver_ptr dsp;
#endif // __loaded__dsp_drivers_h__
