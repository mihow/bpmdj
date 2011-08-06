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
#ifndef __loaded__user_notification_cpp__
#define __loaded__user_notification_cpp__
using namespace std;
#include <qmessagebox.h>
#include "bpmdj.h"
#include "user-notification.h"

UserNotification::UserNotification(): 
  update_view(false), main_changed(false), 
  status(""), status_time(2000), title(""), error(""),
  update_item_list(false)
{
}

void UserNotification::updateProcessView(bool mc)
{
  main_changed=mc;
  update_view=true;
}

void UserNotification::send()
{
  if (app)
    app->postEvent(selector,this);
}

void UserNotification::run()
{
  if (!status.isEmpty())
    ::status->showMessage(status,status_time);
  if (!error.isEmpty())
    QMessageBox::warning(selector,title,error);
  if (update_item_list)
    selector->updateItemList();
  if (update_view)
    selector->updateProcessView(main_changed);
};
#endif // __loaded__user_notification_cpp__
