/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
  DecoderChecker(int decoder, QString extension) : ext(extension), decoder_to_use(decoder)
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
    size = noise_ogg_size;
    return noise_ogg;
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
      size = noise_mp3_size;
      return noise_mp3;
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
