/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__bpmdj_pref_h__
#define __loaded__bpmdj_pref_h__
using namespace std;
#line 1 "bpmdj-pref.h++"
#include "ui-bpmdj-pref.h"
#include "ui-installremotes.h"

class Ui::InstallRemotes;

class BpmDjPreferences : 
  public QDialog,
  public Ui::BpmDjPreferences
{
   Q_OBJECT
 private:
   void fixColorOf(QWidget *p);
   QString getHostFor(QString command);
   void  installRemotes(Ui::InstallRemotes * dialog);
   void  installRemotes();
   void  copyProgramTo(QString program, QString host, Ui::InstallRemotes * dialog);
 public:
   BpmDjPreferences(QWidget*parent=0,const char*name=0, bool modal=FALSE, Qt::WindowFlags f=0);
 public slots:
   // player & anal installation options
   virtual void on_playerInstallButton_clicked() { installRemotes(); };
   virtual void on_analyzerInstallButton_clicked() { installRemotes(); };
   virtual void on_colorTempo44_clicked();
   virtual void on_colorTempo54_clicked();
   virtual void on_colorTempo64_clicked();
   virtual void on_colorTempo74_clicked();
   virtual void on_colorTempo84_clicked();
   virtual void on_greenTimeColor_clicked();
   virtual void on_yellowTimeColor_clicked();
   virtual void on_orangeTimeColor_clicked();
   virtual void on_redTimeColor_clicked();
   virtual void on_colorPlayedSong_clicked();
   virtual void on_colorPlayedAuthor_clicked();
   virtual void on_colorUnavailableSong_clicked();
   virtual void on_colordColorCol_clicked();
   virtual void on_colorAlltimePlaycount_clicked();
};
#endif // __loaded__bpmdj_pref_h__
