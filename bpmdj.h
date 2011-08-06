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
#ifndef __loaded__bpmdj_h__
#define __loaded__bpmdj_h__
using namespace std;
#include <qapplication.h>
#include <qsplashscreen.h>
#include <qpixmap.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qmutex.h>
#include "Data/data-version.h"
#include "fragment-player.h"
#include "spectrum-pca.h"
#include "existence-scanner.h"
#include "clock-drivers.h"
#include "index-reader.h"

#if (DATA_VERSION_MAJOR != 0) || (DATA_VERSION_MINOR != 2)
#error "DATA has wrong version number"
#endif

extern QApplication * app;
extern const char*    programname;
extern QStatusBar*    status;
/**
 * The active object that will play fragments
 */
extern FragmentPlayer* fragmentPlayer;
/**
 * The active object that will run a principal component analysis on the 
 * spectrum property.
 */
extern SpectrumPca*     spectrumPca; 
/**
 * The active object that scans the existence of files
 */
extern ExistenceScanner* existenceScanner;
extern IndexReader*      indexReader;
#endif // __loaded__bpmdj_h__
