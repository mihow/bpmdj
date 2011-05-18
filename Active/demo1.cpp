/****
 Active Objects v4.3
 Copyright (C) 2006-2011 Werner Van Belle
 http://active.yellowcouch.org/

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
#ifndef __loaded__demo1_cpp__
#define __loaded__demo1_cpp__
using namespace std;
#line 1 "demo1.c++"
#define TRACE_MESSAGES
#include "demo1.h"

elementResult ActiveDemoSender::startSending(DemoReceiver* recv, int a)
{
  tosend = a;
  for(int i = 0 ; i < tosend ;  i++)
    recv->printNumber(i);
  deactivate();
  return Done;
}

elementResult ActiveDemoReceiver::printNumber(int nr)
{
   printf("%d  ",nr);
   fflush(stdout);
   if (nr==20) deactivate();
   return Done;
}

DemoSender sender;
DemoReceiver recv;

int main(int, char* [])
{
  sender.startSending(&recv,100);
  sleep(10);
}
#endif // __loaded__demo1_cpp__
