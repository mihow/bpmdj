/****
 BpmDj v4.2 beta: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__info_h__
#define __loaded__info_h__
using namespace std;
#line 1 "info.h++"
/**
 * Todo: 
 * To convert these to Qt versions means that each of the 
 * arguments passed into the ... must still be a char* pointer
 * since we cannot guarantee that, we might need to rethink the
 * layout of these functions. QString provides some nice
 * alternatives, but we will still need to convert everything.
 */
void Log(const char* prefix, const char* text);
void Debug(const char* script,...);
void Info(const char* script,...);
void Warning(const char* script,...);
void Error(bool ui, const char* script,...);
/**
 * Fatal prints an error message and will then exit the application with 
 * an error code of 100.
 */
void Fatal(const char* script,...);
void Remote(const char* script,...);

#endif // __loaded__info_h__
