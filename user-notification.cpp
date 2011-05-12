/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
using namespace std;
#line 1 "user-notification.c++"
#include <qmessagebox.h>
#include "bpmdj.h"
#include "user-notification.h"

UserNotification::UserNotification(): update_view(false), main_changed(false), 
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
    app->postEvent(song_selector_window,this);
}

void UserNotification::run(SongSelectorLogic * window)
{
  if (!status.isEmpty())
    ::status->message(status,status_time);
  if (!error.isEmpty())
    QMessageBox::warning(window,title,error);
  if (update_item_list)
    window->updateItemList();
  if (update_view)
    window->updateProcessView(main_changed);
};
