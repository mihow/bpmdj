/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

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
#include "preferences.logic.h"
#include "playercommandwizard.h"
PreferencesLogic::PreferencesLogic(QWidget*parent,const char*name, bool modal, WFlags f):
  PreferencesDialog(parent,name,modal,f)
{
}

void PreferencesLogic::createPlayerOne()
{
  QString command = getCommand();
  if (command.isNull()) return;
  playerCommand1->setText(command);
}

void PreferencesLogic::createPlayerTwo()
{
  QString command = getCommand();
  if (command.isNull()) return;
  playerCommand2->setText(command);
}

void PreferencesLogic::createPlayerThree()
{
  QString command = getCommand();
  if (command.isNull()) return;
  playerCommand3->setText(command);
}

void PreferencesLogic::createPlayerFour()
{
  QString command = getCommand();
  if (command.isNull()) return;
  playerCommand4->setText(command);
}

QString PreferencesLogic::getCommand()
{
  PlayerCommandWizard commandCreator(NULL,NULL,TRUE);
  if (commandCreator.exec()!=QDialog::Accepted)
    return "";

  // xmms player
  if (commandCreator.xmms->isChecked())
    return "xmms-play \"%s\" -m \"%s\"";
  
  // the common options
  QString options = "kbpm-play ";
  QString text = commandCreator.latency->text();
  if (!text.isNull())
    options+="-L "+text+" ";
  text = commandCreator.x->text();
  if (!text.isNull())
    options+="-p "+text+" "+commandCreator.y->text()+" ";
  if (commandCreator.verbose->isChecked())
    options+="-v ";
  
  // the player specific options
  if (commandCreator.none->isChecked())
    {
      options+="--none ";
    }
  else if (commandCreator.oss->isChecked())
    {
      options+="--oss ";
      text = commandCreator.dsp->text();
      if (!text.isNull())
	options+="--dsp "+text+" ";
      text = commandCreator.fragments->text();
      if (!text.isNull())
	options+="--fragments "+text+" ";
      if (commandCreator.nolatency->isChecked())
	options+="-X ";
    }
  else if (commandCreator.alsa->isChecked())
    {
      options+="--alsa ";
      text = commandCreator.dev->text();
      if (!text.isNull())
	options+="--dev "+text+" ";
    }
  else
    {
      QMessageBox::message(NULL,"please select an OSS or ALSA driver\n");
      return "";
    }
  // creation of the command line
  options+="-r \"%s\" -m \"%s\" \"%s\"";
  return options;
}
