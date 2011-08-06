/****
 BpmDj v4.2-pl2: Free Dj Tools
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
#ifndef __loaded__user_notification_h__
#define __loaded__user_notification_h__
using namespace std;
#include <qstring.h>
#include "bpmdj-event.h"
#include "selector.h"

/**
 * A user notification can contain information for dialog boxes and status
 * messages. It can also contain a flag that updates the status bar if needed.
 * To use it you should always use a new operator and never take the address
 * of a temporary or static notification. The reason is that the notification 
 * itself is deleted by the receiver at arrival.
 */
class UserNotification: public BpmDjEvent
{
  bool update_view;
  bool main_changed;
public:
  QString status;
  int status_time;
  QString title;
  QString error;
  bool update_item_list;
  UserNotification();
  void updateProcessView(bool mc);
  void send();
  virtual void run();
};
#endif // __loaded__user_notification_h__
