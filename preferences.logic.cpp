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
using namespace std;
#line 1 "preferences.logic.c++"
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qwidgetstack.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <qcolordialog.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include "kbpm-dj.h"
#include "preferences.logic.h"
#include "scripts.h"
#include "common.h"
#include "installbpmplay.h"
#include "config.h"

PreferencesLogic::PreferencesLogic(QWidget*parent,const char*name, bool modal, WFlags f):
  PreferencesDialog(parent,name,modal,f)
{
  QWidget * w = widgetStack->widget(2);
  for(int i = 0 ; i < 8; i ++)
    analyzerStack->add(new SongProcPrefView(w,Config::analyzers[i])); 
  w = widgetStack->widget(0);
  for(int i = 0 ; i < 4 ; i++)
    playerStack->add(new SongProcPrefView(w,Config::players[i]));
}

void PreferencesLogic::fixColorOf(QWidget *p)
{
  QColor c = p->paletteBackgroundColor();
  c = QColorDialog::getColor(c,this);
  p->setPaletteBackgroundColor(c);
}

void PreferencesLogic::modifyAlltimeColor()
{
  fixColorOf(colorAlltimePlaycount);
}

void PreferencesLogic::modifyTempo44Color()
{
  fixColorOf(colorTempo44);
}

void PreferencesLogic::modifyTempo54Color()
{
  fixColorOf(colorTempo54);
}

void PreferencesLogic::modifyTempo64Color()
{
  fixColorOf(colorTempo64);
}

void PreferencesLogic::modifyTempo74Color()
{
  fixColorOf(colorTempo74);
}

void PreferencesLogic::modifyTempo84Color()
{
  fixColorOf(colorTempo84);
}

void PreferencesLogic::modifyGreenTime()
{
  fixColorOf(greenTimeColor);
}

void PreferencesLogic::modifyYellowTime()
{
  fixColorOf(yellowTimeColor);
}

void PreferencesLogic::modifyOrangeTime()
{
  fixColorOf(orangeTimeColor);
}

void PreferencesLogic::modifyRedTime()
{
  fixColorOf(redTimeColor);
}

void PreferencesLogic::modifyPlayedColor()
{
  fixColorOf(colorPlayedSong);
}

void PreferencesLogic::modifyAuthorColor()
{
  fixColorOf(colorPlayedAuthor);
}

void PreferencesLogic::modifyUnavailableColor()
{
  fixColorOf(colorUnavailableSong);
}

void PreferencesLogic::modifydColorCol()
{
  fixColorOf(colordColorCol);
}

void PreferencesLogic::copyProgramTo(QString program, QString host, InstallBpmPlay * dialog)
{
  QString command = QString("scp -B ")+program+" "+host+":";
  QString makeexec = QString("ssh ")+host+" chmod +rx kbpm-play bpmdj-raw";

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
  QString log_line;
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

void PreferencesLogic::installRemotes(InstallBpmPlay * dialog)
{
  /** 
   * find the positions of the programs
   */
  QString location_kbpmplay = exists("./kbpm-play") ? "./kbpm-play" :
    exists("/usr/bin/kbpm-play") ? "/usr/bin/kbpm-play" : ";";
  QString location_bpmdjraw = exists("./bpmdj-raw") ? "./bpmdj-raw" :
    exists("/usr/bin/bpmdj-raw") ? "/usr/bin/bpmdj-raw" : "";
  if (location_kbpmplay.isEmpty())
    {
      QMessageBox::message("Install","I cannot find the kbpm-play binary\n");
      return;
    }
  if (location_bpmdjraw.isEmpty())
    {
      QMessageBox::message("Install","I cannot find the bpmdj-raw script\n");
      return;
    }
  dialog->setModal(true);
  dialog->show();
  
  /**
   * copy them to the remote host
   */
  QListViewItemIterator it(dialog->hosts);
  while(*it)
    {
      QListViewItem *i = *it;
      QString host = i -> text(0);
      QString program_files = location_kbpmplay+" "+location_bpmdjraw;
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

void PreferencesLogic::installRemotes()
{
  InstallBpmPlay dialog;
  QString remote;
  for(int i = 0 ; i < 4; i++)
    {
      remote = Config::players[i].getRemote();
      if(!remote.isEmpty() && !dialog.hosts->findItem(remote,0)) 
	new QListViewItem(dialog.hosts,remote);
    }
  for(int i = 0 ; i < 8; i++)
    {
      remote = Config::analyzers[i].getRemote();
      if(!remote.isEmpty() && !dialog.hosts->findItem(remote,0)) 
	new QListViewItem(dialog.hosts,remote);
    }
  if (dialog.hosts->childCount()==0)
    {
      QMessageBox::message("Install","Install is only necessary for remote players\n"
			   "There seem to be no remote players present.\n");
      return;
    }
  installRemotes(&dialog);
  dialog.exec();
};
