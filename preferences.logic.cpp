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
#include <qcolordialog.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include "preferences.logic.h"
#include "playercommandwizard.h"

PreferencesLogic::PreferencesLogic(QWidget*parent,const char*name, bool modal, WFlags f):
  PreferencesDialog(parent,name,modal,f)
{
}

void PreferencesLogic::createPlayerOne()
{
  QString command = getCommand(0,0);
  if (command.isNull()) return;
  playerCommand1->setText(command);
}

void PreferencesLogic::createPlayerTwo()
{
  QString command = getCommand(640,0);
  if (command.isNull()) return;
  playerCommand2->setText(command);
}

void PreferencesLogic::createPlayerThree()
{
  QString command = getCommand(0,200);
  if (command.isNull()) return;
  playerCommand3->setText(command);
}

void PreferencesLogic::createPlayerFour()
{
  QString command = getCommand(640,200);
  if (command.isNull()) return;
  playerCommand4->setText(command);
}

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


QString PreferencesLogic::getCommand(int x_suggestion, int y_suggestion)
{
  PlayerCommandWizard commandCreator(NULL,NULL,TRUE);
  commandCreator.x->setText(QString::number(x_suggestion));
  commandCreator.y->setText(QString::number(y_suggestion));
  
  if (commandCreator.exec()!=QDialog::Accepted)
    return "";
  
  // xmms player
  if (commandCreator.xmms->isChecked())
    return "xmms-play \"%s\" -m \"%s\"";
  
  // the common options
  QString options = "kbpm-play ";
  if (!commandCreator.remote->text().isEmpty())
    options += "--remote "+commandCreator.remote->text()+" ";

  QString text = commandCreator.x->text();
  if (!text.isNull())
    options+="-p "+text+" "+commandCreator.y->text()+" ";

  if (!commandCreator.rms->text().isEmpty())
    options+="--rms "+commandCreator.rms->text()+" ";
  
  // the player specific options
  if (commandCreator.none->isChecked())
    {
      options+="--none ";
    }
  else if (commandCreator.bpm->isChecked())
    {
      options+="--mixed "+commandCreator.bpmchannel->text()+" ";
      if (!commandCreator.remote->text().isEmpty())
	QMessageBox::information(NULL,
				 "kbpm-mix / remote conflict",
				 "You filled in a remote option (Remote)\n"
				 "and also selected a local bpm mixer (Bpm Local Mix)\n"
				 "These two are incompatible since the player will send\n" 
				 "data to the remote local channel",
				 QMessageBox::Ok,QMessageBox::NoButton);
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

  // some more common options
  text = commandCreator.latency->text();
  if (!text.isNull()
      && !commandCreator.none->isChecked())
    options+="-L "+text+" ";
  if (commandCreator.verbose->isChecked() 
      && !commandCreator.none->isChecked())
    options+="-v ";

  // creation of the command line
  options+="-r \"%s\" -m \"%s\" \"%s\"";
  return options;
}
