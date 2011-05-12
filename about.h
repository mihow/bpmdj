/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__about_h__
#define __loaded__about_h__
using namespace std;
#line 1 "about.h++"
#include "version.h"
#include "ui-about.h"

void doAbout(int pg);

class About: 
  public QDialog,
  public Ui_About
{
  Q_OBJECT
public:
  About(int pg): QDialog(0,0,1)
  {
    setupUi(this);
    char tmp[500];
    sprintf(tmp,"BpmDj v%s",VERSION);
    versionLabel->setText(tmp);
    pages->setCurrentPage(pg);
    exec();
  }
};

#endif // __loaded__about_h__
