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

BpmDjPreferences::BpmDjPreferences(QWidget*parent,const char*name, bool modal, Qt::WindowFlags f):
  QDialog(parent,name,modal,f)
{
  setupUi(this);
  //  QWidget * w = widgetStack->widget(2);
  for(int i = 0 ; i < 8; i ++)
    analyzerStack->layout()->add(new SongProcPrefView(analyzerStack,Config::analyzers[i]));
  //  w = widgetStack->widget(0);
  for(int i = 0 ; i < 4 ; i++)
    playerStack->layout()->add(new SongProcPrefView(playerStack,Config::players[i]));
}

void BpmDjPreferences::fixColorOf(QWidget *p)
{
  QColor c = p->paletteBackgroundColor();
  c = QColorDialog::getColor(c,this);
  if (c.isValid())
    p->setPaletteBackgroundColor(c);
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

void BpmDjPreferences::copyProgramTo(QString program, QString host, Ui::InstallRemotes * dialog)
{
  QString command = QString("scp -B ")+program+" "+host+":";
  QString makeexec = QString("ssh ")+host+" chmod +rx bpmplay bpmdjraw";

  dialog->output->setBold(true);
  dialog->output->setColor(QColor(255,0,0));
  dialog->output->append(host+"\n");
  dialog->output->setColor(QColor(0,0,0));
  dialog->output->setBold(true);
  dialog->output->append(QString("$ ")+command+"\n");
  dialog->output->setBold(false);
  app->processEvents();

  QFile log("install.log");
  log.open(IO_WriteOnly);
  log.close();
  execute(command+" >>install.log 2>>install.log");
  log.open(IO_ReadOnly);
  char log_line[5000];
  while(log.readLine(log_line,5000)>=0)
    dialog->output->append(log_line);
  log.close();

  dialog->output->setBold(true);
  dialog->output->append(QString("$ ")+makeexec+"\n");
  dialog->output->setBold(false);

  log.open(IO_WriteOnly);
  log.close();
  
  execute(makeexec+" >>install.log 2>>install.log");
  
  log.open(IO_ReadOnly);
  while(log.readLine(log_line,5000)>=0)
    dialog->output->append(log_line);
  log.close();
}

void BpmDjPreferences::installRemotes(Ui::InstallRemotes * dialog)
{
  /** 
   * find the positions of the programs
   */
  QString location_bpmplay = exists("./bpmplay") ? "./bpmplay" :
    exists("/usr/bin/bpmplay") ? "/usr/bin/bpmplay" : ";";
  QString location_bpmdjraw = exists("./bpmdjraw") ? "./bpmdjraw" :
    exists("/usr/bin/bpmdjraw") ? "/usr/bin/bpmdjraw" : "";
  if (location_bpmplay.isEmpty())
    {
      QMessageBox::message("Install","I cannot find the bpmplay binary\n");
      return;
    }
  if (location_bpmdjraw.isEmpty())
    {
      QMessageBox::message("Install","I cannot find the pmdjraw script\n");
      return;
    }

  /**
   * copy them to the remote host
   */
  Q3ListViewItemIterator it(dialog->hosts);
  while(*it)
    {
      Q3ListViewItem *i = *it;
      QString host = i -> text(0);
      QString program_files = location_bpmplay+" "+location_bpmdjraw;
      copyProgramTo(program_files,host,dialog);
      it++;
    }
  
  /**
   * inform user of finish
   */
  dialog->output->setBold(true);
  dialog->output->setColor(QColor(255,0,0));
  dialog->output->append("Finished\n");
  dialog->output->setColor(QColor(0,0,0));
  dialog->okbutton->setEnabled(true);
}

void BpmDjPreferences::installRemotes()
{
  QDialog container;
  container.setModal(true);
  Ui::InstallRemotes dialog;
  dialog.setupUi(&container);
  container.show();
  QString remote;
  for(int i = 0 ; i < 4; i++)
    {
      remote = Config::players[i].getRemote();
      if(!remote.isEmpty() && !dialog.hosts->findItem(remote,0)) 
	new Q3ListViewItem(dialog.hosts,remote);
    }
  for(int i = 0 ; i < 8; i++)
    {
      remote = Config::analyzers[i].getRemote();
      if(!remote.isEmpty() && !dialog.hosts->findItem(remote,0)) 
	new Q3ListViewItem(dialog.hosts,remote);
    }
  if (dialog.hosts->childCount()==0)
    {
      QMessageBox::message("Install","Install is only necessary for remote players\n"
			   "There seem to be no remote players present.\n");
      return;
    }
  installRemotes(&dialog);
  container.exec();
};
#endif // __loaded__bpmdj_pref_cpp__
