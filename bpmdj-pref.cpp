/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__bpmdj_pref_cpp__
#define __loaded__bpmdj_pref_cpp__
using namespace std;
#line 1 "bpmdj-pref.c++"
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <qcolordialog.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include "bpmdj.h"
#include "bpmdj-pref.h"
#include "scripts.h"
#include "common.h"
#include "config.h"
#include "qt-helpers.h"

BpmDjPreferences::BpmDjPreferences(QWidget*parent): QDialog(parent)
{
  setupUi(this);
  //  QWidget * w = widgetStack->widget(2);
  for(int i = 0 ; i < 8; i ++)
    analyzerStack->layout()->addWidget(new SongProcPrefView(analyzerStack,
						    Config::analyzers[i]));
  //  w = widgetStack->widget(0);
  for(int i = 0 ; i < 4 ; i++)
    playerStack->layout()->addWidget(new SongProcPrefView(playerStack,
						    Config::players[i]));
}

void BpmDjPreferences::fixColorOf(QWidget *p)
{
  QPalette P=p->palette();
  QColor c=P.color(p->backgroundRole());
  c = QColorDialog::getColor(c,this);
  if (c.isValid())
    {
      P.setColor(p->backgroundRole(),c);
      p->setPalette(P);
    }
}

void BpmDjPreferences::on_colorAlltimePlaycount_clicked()
{
  fixColorOf(colorAlltimePlaycount);
}

void BpmDjPreferences::on_colorTempo44_clicked()
{
  fixColorOf(colorTempo44);
}

void BpmDjPreferences::on_colorTempo54_clicked()
{
  fixColorOf(colorTempo54);
}

void BpmDjPreferences::on_colorTempo64_clicked()
{
  fixColorOf(colorTempo64);
}

void BpmDjPreferences::on_colorTempo74_clicked()
{
  fixColorOf(colorTempo74);
}

void BpmDjPreferences::on_colorTempo84_clicked()
{
  fixColorOf(colorTempo84);
}

void BpmDjPreferences::on_greenTimeColor_clicked()
{
  fixColorOf(greenTimeColor);
}

void BpmDjPreferences::on_yellowTimeColor_clicked()
{
  fixColorOf(yellowTimeColor);
}

void BpmDjPreferences::on_orangeTimeColor_clicked()
{
  fixColorOf(orangeTimeColor);
}

void BpmDjPreferences::on_redTimeColor_clicked()
{
  fixColorOf(redTimeColor);
}

void BpmDjPreferences::on_colorPlayedSong_clicked()
{
  fixColorOf(colorPlayedSong);
}

void BpmDjPreferences::on_colorPlayedAuthor_clicked()
{
  fixColorOf(colorPlayedAuthor);
}

void BpmDjPreferences::on_colorUnavailableSong_clicked()
{
  fixColorOf(colorUnavailableSong);
}

void BpmDjPreferences::on_colordColorCol_clicked()
{
  fixColorOf(colordColorCol);
}

void BpmDjPreferences::copyProgramTo(QString program, QString host, 
				     Ui_InstallRemotes * dialog)
{
  QString command = QString("scp -B ")+program+" "+host+":";
  QString makeexec = QString(SSH)+host+" chmod +rx bpmplay bpmdjraw";
  
  dialog->output->setFontWeight(QFont::Bold);
  dialog->output->setTextColor(QColor(255,0,0));
  dialog->output->append(host+"\n");
  dialog->output->setTextColor(QColor(0,0,0));
  dialog->output->append(QString("$ ")+command+"\n");
  dialog->output->setFontWeight(QFont::Normal);
  app->processEvents();

  QFile log("install.log");
  log.open(QIODevice::WriteOnly);
  log.close();
  execute("Install "+command+" at "+host,
	  command+" >>install.log 2>>install.log");
  log.open(QIODevice::ReadOnly);
  char log_line[5000];
  while(log.readLine(log_line,5000)>=0)
    dialog->output->append(log_line);
  log.close();

  dialog->output->setFontWeight(QFont::Bold);
  dialog->output->append(QString("$ ")+makeexec+"\n");
  dialog->output->setFontWeight(QFont::Normal);

  log.open(QIODevice::WriteOnly);
  log.close();
  
  execute("Making "+command+" at "+host+" executable",
	  makeexec+" >>install.log 2>>install.log");
  
  log.open(QIODevice::ReadOnly);
  while(log.readLine(log_line,5000)>=0)
    dialog->output->append(log_line);
  log.close();
}

void BpmDjPreferences::installRemotes(Ui_InstallRemotes * dialog)
{
  /** 
   * find the positions of the programs
   */
  QString location_bpmplay = exists("./bpmplay") ? "./bpmplay" :
    exists("/usr/bin/bpmplay") ? "/usr/bin/bpmplay" : "";
  QString location_bpmdjraw = exists("./bpmdjraw") ? "./bpmdjraw" :
    exists("/usr/bin/bpmdjraw") ? "/usr/bin/bpmdjraw" : "";
  if (location_bpmplay.isEmpty())
    {
      QMessageBox::warning(this,"Install","I cannot find the bpmplay binary");
      return;
    }
  if (location_bpmdjraw.isEmpty())
    {
      QMessageBox::warning(this,"Install","I cannot find the bpmdjraw script");
      return;
    }

  /**
   * Copy them to the remote host
   */
  stdTreeWidgetIterator i(dialog->hosts);
ITERATE_OVER(i)

      QString host = i.val() -> text(0);
      QString program_files = location_bpmplay+" "+location_bpmdjraw;
      copyProgramTo(program_files,host,dialog);
  }
  
  /**
   * Inform user of finish
   */
  dialog->output->setFontWeight(QFont::Bold);
  dialog->output->setTextColor(QColor(255,0,0));
  dialog->output->append("Finished\n");
  dialog->output->setTextColor(QColor(0,0,0));
  dialog->okbutton->setEnabled(true);
}

void BpmDjPreferences::installRemotes()
{
  QDialog container;
  container.setModal(true);
  Ui_InstallRemotes dialog;
  dialog.setupUi(&container);
  container.show();
  QString remote;
  for(int i = 0 ; i < 4; i++)
    {
      remote = Config::players[i].getRemote();
      if(!remote.isEmpty() && !findStdItem(dialog.hosts,remote,0))
	new QTreeWidgetItem(dialog.hosts,QStringList(remote));
    }
  for(int i = 0 ; i < 8; i++)
    {
      remote = Config::analyzers[i].getRemote();
      if(!remote.isEmpty() && !findStdItem(dialog.hosts,remote,0)) 
	new QTreeWidgetItem(dialog.hosts,QStringList(remote));
    }
  if (dialog.hosts->topLevelItemCount()==0)
    {
      QMessageBox::information(this,"Install",
		   "Install is only necessary for remote players\n"
		   "There seem to be no remote players present.");
      return;
    }
  installRemotes(&dialog);
  container.exec();
};
#endif // __loaded__bpmdj_pref_cpp__
