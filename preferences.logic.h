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

#include "preferences.h"

class PreferencesLogic : 
  public PreferencesDialog
{
   Q_OBJECT
 private:
   void fixColorOf(QWidget *p);
   QString getCommand(int x_suggestion, int y_suggestion);
 public:
   PreferencesLogic::PreferencesLogic(QWidget*parent=0,const char*name=0, bool modal=FALSE,WFlags f=0);
 public slots:
   virtual void createPlayerOne();
   virtual void createPlayerTwo();
   virtual void createPlayerThree();
   virtual void createPlayerFour();
   virtual void modifyTempo44Color();
   virtual void modifyTempo54Color();
   virtual void modifyTempo64Color();
   virtual void modifyTempo74Color();
   virtual void modifyTempo84Color();
   virtual void modifyGreenTime();
   virtual void modifyYellowTime();
   virtual void modifyOrangeTime();
   virtual void modifyRedTime();
   virtual void modifyPlayedColor();
   virtual void modifyAuthorColor();
   virtual void modifyUnavailableColor();
   virtual void modifydColorCol();
};
