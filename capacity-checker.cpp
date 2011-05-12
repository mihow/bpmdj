/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__capacity_checker_cpp__
#define __loaded__capacity_checker_cpp__
using namespace std;
#line 1 "capacity-checker.c++"
#include <Qt/qfile.h>
#include <Qt/qtextstream.h>
#include <cstdio>
#include "capacity-checker.h"
#include "embedded-files.h"
#include "common.h"
#include "bpmplay.h"
#include "scripts.h"
#include "info.h"

bool DecoderChecker::works()
{
  bool success = false;
  int progr_nr = capacity_to_prognr(decoder_to_use);
  Info("Checking program %d: %s",progr_nr,ext.toAscii().data());
  // determine unique filename
  int pid = getpid();
  int nr = 0;
  char filename[1000];
  do
    {
      sprintf(filename,"%d-%d.%s",pid,++nr,ext.toAscii().data());
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
      bpmdjraw(true,filename,get_rawpath());
      // gather the output of the writing process [NYI]
      // check the existence of the target raw file
      char * wave_name=getRawFilename(get_rawpath().toAscii().data(),filename);
      FILE * raw_file = fopen(wave_name,"rb");
      if(raw_file)
	{
	  fseek(raw_file,0,SEEK_END);
	  unsigned8 L = ftell(raw_file);
	  if (L>320*1024) success = true; // 340K is the size of the sound logo
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
  vexecute("mpg123 2>%s",log.toAscii().data());
  QFile logfile(log);
  if (!logfile.open(QIODevice::ReadOnly)) return false;
  QTextStream stream(&logfile);
  QString line;
  bool check1=false;
  bool check2=false;
  while(!(line=stream.readLine()).isNull())
    {
      if (line.indexOf("0.59r")>=0) check1=true;
      if (line.indexOf("0.6")>=0) check1=true;
      if (line.indexOf("1.4.3")>=0) check1=true;
      if (line.indexOf("Michael Hipp")>=0) check2=true;
    }
  logfile.close();
  remove(log);
  return check1 && check2;
}

bool Mpg321Decoder::checkProgram(QString filename)
{
  QString log=filename+".log";
  vexecute("mpg321 2>%s",log.toAscii().data());
  QFile logfile(log);
  if (!logfile.open(QIODevice::ReadOnly)) return false;
  QTextStream stream(&logfile);
  QString line;
  bool check1=false;
  bool check2=false;
  while(!(line=stream.readLine()).isNull())
    {
      if (line.indexOf("mpg321")>=0) check1=true;
      if (line.indexOf("Joe Drew")>=0) check2=true;
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
  if (!Mp3Decoder(CAPACITY_MPLAYER1PRE6).works()) 
    unavailable_capacities |= CAPACITY_MPLAYER1PRE6;
  if (!Mp3Decoder(CAPACITY_MPLAYER1PRE7).works()) 
    unavailable_capacities |= CAPACITY_MPLAYER1PRE7;
}
#endif // __loaded__capacity_checker_cpp__
