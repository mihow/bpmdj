/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#include "kbpm-dj.h"
#include "preferences.logic.h"
#include "scripts.h"
#include "common.h"
#include "playercommandwizard.h"
#include "analcommandwizard.h"
#include "installbpmplay.h"

PreferencesLogic::PreferencesLogic(QWidget*parent,const char*name, bool modal, WFlags f):
  PreferencesDialog(parent,name,modal,f)
{
}

void PreferencesLogic::createPlayerOne()
{
  QString command = getCommand("player1");
  if (command.isNull()) return;
  playerCommand1->setText(command);
}

void PreferencesLogic::createPlayerTwo()
{
  QString command = getCommand("player2");
  if (command.isNull()) return;
  playerCommand2->setText(command);
}

void PreferencesLogic::createPlayerThree()
{
  QString command = getCommand("player3");
  if (command.isNull()) return;
  playerCommand3->setText(command);
}

void PreferencesLogic::createPlayerFour()
{
  QString command = getCommand("player4");
  if (command.isNull()) return;
  playerCommand4->setText(command);
}

#define ANAL(XX) void PreferencesLogic::createAnal##XX() { \
  QString command = getAnal("analyzer"#XX); \
  if (command.isEmpty()) return; \
  analyzerCommand##XX->setText(command); }
ANAL(1);
ANAL(2);
ANAL(3);
ANAL(4);
ANAL(5);
ANAL(6);
ANAL(7);
ANAL(8);
#undef ANAL

void PreferencesLogic::fixColorOf(QWidget *p)
{
  QColor c = p->paletteBackgroundColor();
  c = QColorDialog::getColor(c,this);
  p->setPaletteBackgroundColor(c);
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

QString PreferencesLogic::getCommand(QString name_suggestion)
{
  PlayerCommandWizard commandCreator(NULL,NULL,TRUE);
  commandCreator.player_name->setText(name_suggestion);
  
  if (commandCreator.exec()!=QDialog::Accepted)
    return "";
  
  // xmms player
  if (commandCreator.xmms->isChecked())
    return "xmms-play \"%s\" -m \"%s\"";
  
  // the common options
  QString options = "kbpm-play ";
  if (!commandCreator.remote->text().isEmpty())
    options += "--remote "+commandCreator.remote->text()+" ";
  options += "--config "+commandCreator.player_name->text();

  if (commandCreator.verbose->isChecked() 
      && !commandCreator.none->isChecked())
    options+="-v ";
  
  // creation of the command line
  options+=" -m \"%s\" \"%s\"";
  return options;
}

QString PreferencesLogic::getAnal(QString name_suggestion)
{
  AnalyzerCommandWizard commandCreator(NULL,NULL,TRUE);
  commandCreator.player_name->setText(name_suggestion);
  
  if (commandCreator.exec()!=QDialog::Accepted)
    return "";
  
  // the common options
  QString options = "kbpm-play ";
  if (!commandCreator.remote->text().isEmpty())
    options += "--remote "+commandCreator.remote->text()+" ";
  options += "--config "+commandCreator.player_name->text();
  
  // creation of the command line
  options+=" ";
  return options;
}

void PreferencesLogic::copyProgramTo(QString program, QString host, InstallBpmPlay * dialog)
{
  QString command = QString("scp -B ")+program+" "+host+":";
  dialog->output->setBold(true);
  dialog->output->setColor(QColor(255,0,0));
  dialog->output->append(host+"\n");
  dialog->output->setColor(QColor(0,0,0));
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
      copyProgramTo(location_kbpmplay+" "+location_bpmdjraw,host,dialog);
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
  QString command;
#define PLAYER(XX) command = getHostFor(playerCommand##XX ->text()); \
  if(!command.isEmpty() && !dialog.hosts->findItem(command,0)) \
    new QListViewItem(dialog.hosts,command);
PLAYER(1);
PLAYER(2);
PLAYER(3);
PLAYER(4);
#undef PLAYER

#define ANAL(XX) command = getHostFor(analyzerCommand##XX ->text()); \
  if(!command.isEmpty() && !dialog.hosts->findItem(command,0)) \
    new QListViewItem(dialog.hosts,command);
ANAL(1);
ANAL(2);
ANAL(3);
ANAL(4);
ANAL(5);
ANAL(6);
ANAL(7);
ANAL(8);
#undef ANAL
 if (dialog.hosts->childCount()==0)
   {
     QMessageBox::message("Install","Install is only necessary for remote players\n"
			  "There seem to be no remote players present.\n");
     return;
   }
 installRemotes(&dialog);
 dialog.exec();
};


QString PreferencesLogic::getHostFor(QString command)
{
  int idx = command.find("--remote ");
  if (idx<0) return "";
  QString host = command.mid(idx+QString("--remote ").length());
  host = host.stripWhiteSpace();
  idx = host.find(' ');
  if (idx) host=host.left(idx);
  host = host.stripWhiteSpace();
  return host;
}

#define PLAYER(XX) void PreferencesLogic::setupPlayer##XX () { \
  setup(playerCommand##XX ->text()); }
#define ANAL(XX) void PreferencesLogic::setupAnal##XX () { \
  setup(analyzerCommand##XX ->text()); }
PLAYER(1);
PLAYER(2);
PLAYER(3);
PLAYER(4);
ANAL(1);
ANAL(2);
ANAL(3);
ANAL(4);
ANAL(5);
ANAL(6);
ANAL(7);
ANAL(8);
#undef ANAL
#undef PLAYER

void PreferencesLogic::setup(QString commandline)
{
  commandline+=" --setup";
  spawn(commandline);
}
