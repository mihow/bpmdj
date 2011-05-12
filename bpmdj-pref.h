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
#ifndef __loaded__bpmdj_pref_h__
#define __loaded__bpmdj_pref_h__
using namespace std;
#line 1 "bpmdj-pref.h++"
#include "ui-bpmdj-pref.h"
#include "ui-installremotes.h"

class Ui_InstallRemotes;

class BpmDjPreferences : 
  public QDialog,
  public Ui_BpmDjPreferences
{
  Q_OBJECT
private:
  void fixColorOf(QWidget *p);
  QString getHostFor(QString command);
  void  installRemotes(Ui_InstallRemotes * dialog);
  void  installRemotes();
  void  copyProgramTo(QString program, QString host, 
		      Ui_InstallRemotes * dialog);
public:
  BpmDjPreferences(QWidget*parent=0);
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
