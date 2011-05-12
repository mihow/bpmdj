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
#line 1 "capacity-checker.c++"
#include <qfile.h>
#include "capacity-checker.h"
#include "embedded-files.h"
#include "common.h"
#include "kbpm-play.h"
#include "scripts.h"

bool DecoderChecker::works()
{
  bool success = false;
  int progr_nr = capacity_to_prognr(decoder_to_use);
  Info("Checking program %d: %s",progr_nr,(const char*)ext);
  // determine unique filename
  int pid = getpid();
  int nr = 0;
  char filename[1000];
  do
    {
      nr++;
      sprintf(filename,"%d-%d.%s",pid,nr,(const char*)ext);
    }
  while (exists(filename));
  if (checkProgram(filename)) 
    {
      // write a chunk of data to disk
      FILE * coded_file = fopen(filename,"wb");
      assert(coded_file);
      int coded_size = 0;
      const char * coded = getCodedData(coded_size);
      assert(coded);
      assert(coded_size);
      int written = fwrite(coded,coded_size,1,coded_file);
      assert(written==1);
      fclose(coded_file);
      // set the decoder environment to pick only the wanted decoder
      char environment[50];
      sprintf(environment,"DECODER=%d",progr_nr);
      putenv(strdup(environment));
      // start decoding it
      start_bpmdj_raw(get_rawpath(),filename);
      // gather the output of the writing process [NYI]
      // check the existence of the target raw file
      char * wave_name=getRawFilename(get_rawpath(),filename);
      FILE * raw_file = fopen(wave_name,"rb");
      if(raw_file)
	{
	  fseek(raw_file,0,SEEK_END);
	  unsigned8 L = ftell(raw_file);
	  if (L>350*1024) success = true;
	  fclose(raw_file);
	}
      // remove the .raw file
      remove(wave_name);
      // remove the original file
      remove(filename);
    }
  else
    {
      Info("Program is wrong version");
    }
  // return the success status
  if (success)
    Info("Program %d is available",progr_nr);
  else
    Info("Program %d is unavailable",progr_nr);
  return success;
}

bool Mpg123Decoder::checkProgram(QString filename)
{
  QString log=filename+".log";
  vexecute("mpg123 2>%s",(const char*)log);
  QFile logfile(log);
  if (!logfile.open(IO_ReadOnly)) return false;
  QTextStream stream(&logfile);
  QString line;
  bool check1=false;
  bool check2=false;
  while(!(line=stream.readLine()).isNull())
    {
      if (line.find("0.59r")>=0) check1=true;
      if (line.find("0.60")>=0) check1=true;
      if (line.find("0.61")>=0) check1=true;
      if (line.find("Michael Hipp")>=0) check2=true;
    }
  logfile.close();
  remove(log);
  return check1 && check2;
}

bool Mpg321Decoder::checkProgram(QString filename)
{
  QString log=filename+".log";
  vexecute("mpg321 2>%s",(const char*)log);
  QFile logfile(log);
  if (!logfile.open(IO_ReadOnly)) return false;
  QTextStream stream(&logfile);
  QString line;
  bool check1=false;
  bool check2=false;
  while(!(line=stream.readLine()).isNull())
    {
      if (line.find("mpg321")>=0) check1=true;
      if (line.find("Joe Drew")>=0) check2=true;
    }
  logfile.close();
  remove(log);
  return check1 && check2;
}

void check_capacities()
{
  unavailable_capacities = no_disabled_capacities;
  if (!Ogg123Decoder().works()) unavailable_capacities |= CAPACITY_OGG123;
  if (!Mpg123Decoder().works()) unavailable_capacities |= CAPACITY_MPG123059R;
  if (!Mpg321Decoder().works()) unavailable_capacities |= CAPACITY_MPG321;
  if (!Mp3Decoder(CAPACITY_MPLAYER1PRE6).works()) unavailable_capacities |= CAPACITY_MPLAYER1PRE6;
  if (!Mp3Decoder(CAPACITY_MPLAYER1PRE7).works()) unavailable_capacities |= CAPACITY_MPLAYER1PRE7;
}
