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
#ifndef __BPMDJ___KBPM_DJ_H__
#define __BPMDJ___KBPM_DJ_H__
using namespace std;
#line 1 "kbpm-dj.h++"
#include <qapplication.h>
#include <qsplashscreen.h>
#include <qpixmap.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qmutex.h>
#include "data-version.h"
#include "fragment-player.h"
#include "spectrum-pca.h"
#include "existence-scanner.h"
#if (DATA_VERSION_MAJOR != 0) || (DATA_VERSION_MINOR != 2)
#error "DATA has wrong version number"
#endif

extern QApplication * app;
extern const char*    programname;
extern QStatusBar*    status;
// The active object that will play fragments
extern FragmentPlayer fragmentPlayer;
// The active object that will run a spectrum principal component analysis
extern SpectrumPca    spectrumPca; 
// The active object that scans the existence of files
extern ExistenceScanner existenceScanner;
#endif
