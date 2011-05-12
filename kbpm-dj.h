/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#include <qapplication.h>
#include <qsplashscreen.h>
#include <qpixmap.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qmutex.h>

extern QApplication *app;
extern const char*   programname;
extern QStatusBar*   status;
extern QMutex        bpmdj_busy;

#define EXCLUSIVE QMutexLocker bpmdj_entered(&::bpmdj_busy);
