/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__about_h__
#define __loaded__about_h__
using namespace std;
#line 1 "about.h++"
#include "version.h"
#include "ui-about.h"

void doAbout(int pg);

class About: 
  public QDialog,
  public Ui::About
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
