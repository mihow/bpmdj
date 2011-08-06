/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__capacity_checker_h__
#define __loaded__capacity_checker_h__
using namespace std;
#include "capacity.h"
#include "embedded-files.h"

/**
 * The capacity checker will verify the existence of the given programs
 * and list those that seem unavailable
 */
class DecoderChecker
{
  QString ext;
  int     decoder_to_use;
protected:
  virtual const char *getCodedData(int & size) = 0;
  virtual bool checkProgram(QString filename)
    {
      return true; 
    };
  DecoderChecker(int decoder, QString extension) : 
    ext(extension), decoder_to_use(decoder)
    {
    };
public:
  virtual bool works();
  virtual ~DecoderChecker()
    {
    };
};

void check_capacities();
//---------------------------------------------------------------
class Ogg123Decoder: public DecoderChecker
{
protected:
  virtual const char *getCodedData(int & size)
  {
    size = bpmdj_ogg_size;
    return bpmdj_ogg;
  }
public:
  Ogg123Decoder() : DecoderChecker(CAPACITY_OGG123,"ogg")
  {
  };
};

//---------------------------------------------------------------
class Mp3Decoder: public DecoderChecker
{
 protected:
  virtual const char *getCodedData(int & size)
    {
      size = bpmdj_mp3_size;
      return bpmdj_mp3;
    }
 public:
  Mp3Decoder(int decoder) : DecoderChecker(decoder,"mp3")
  {
  };
};

//---------------------------------------------------------------
class Mpg123Decoder: public Mp3Decoder
{
 protected:
  virtual bool checkProgram(QString);
 public:
  Mpg123Decoder() : Mp3Decoder(CAPACITY_MPG123059R)
    {
    };
};

class Mpg321Decoder: public Mp3Decoder
{
 protected:
  virtual bool checkProgram(QString);
 public:
  Mpg321Decoder() : Mp3Decoder(CAPACITY_MPG321)
    {
    };
};
#endif // __loaded__capacity_checker_h__
