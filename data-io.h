/****
 Om-Data
 Copyright (C) 2005-2006 Werner Van Belle

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

#ifndef DATAIO_H
#define DATAIO_H
using namespace std;
#include "data.h"
#include "data-visitor.h"

//===============================================================
//                         Data Dumper
//===============================================================
class DataIo: public DataVisitor
{
 protected:
  FILE * text;
  bool   opened_file_myself;
  int    version;
 public:
  // constructor
  DataIo(QString fn, const char* mode);
  DataIo(FILE *f);
  // desructors
  virtual ~DataIo(); 
  // read & write virtuals
  Data read();
  virtual void write(Data data) = 0;
  void visit(Data & data);
  virtual void start_writing();
  virtual void start_reading();
 protected:
  virtual void read_into(Data & target) = 0;
  virtual void write_fileformat_versionnr() =0;
  virtual int  read_fileformat_versionnr()  =0;
};

//---------------------------------------------------------------
//                         Data Texter
//---------------------------------------------------------------
class DataTexter: public DataIo
{
 private:
  bool   newline;
  int    lines;
  int    written;
  int    margin;
 private:
  int    get_pos() 
    {
      return written;
    };
  int    get_line() 
    {
      return lines;
    };
  bool   newline_waiting()
    {
      return newline; 
    };
  int    get_margin() 
    {
      return margin;
    };
  void   set_margin(int p) { margin = p;};
  void   set_margin() { set_margin(get_pos()); };
  void   padded(const char* text);
  void   padded(QString c);
  int    push(QString d);
  int    push(const char * d);
  int    push(const char d);
  void   pop(int w, const char* d);
  void   pop(int w, const char d);
  virtual void visitUnsignedNumber(unsigned long long a, char sig);
  virtual void visitSignedNumber(signed long long a, char sig); 
 protected:
  virtual void write_fileformat_versionnr();
  /**
   * returns the fileformat version of the compiled version that saved
   * the data. Returns 0 if it could not be read.
   */
  virtual int  read_fileformat_versionnr();
 protected:
  virtual void visit(Symbol & symbol);
  virtual void visit(Token & token);
  virtual void visit(Float4 & f4);
  virtual void visit(Float8 & f4);

#define ARRAY_TYPE(D,T) \
  virtual void visit(Array<D,T> &array) {visitArray(array);};
  ARRAY_TYPES;
#undef ARRAY_TYPE
  template <int D, class T> void visitArray(Array<D,T> & array);
  template <class T> void dumpSequence(Array<1,T> &array);
  template <class T> void dumpSequence(Array<2,T> &array);
  template <class T> void dumpSequence(Array<3,T> &array);
  template <class T> void dumpSequence(Array<4,T> &array);
  template <class T> void dumpSequence(Array<5,T> &array);
  template <class T> void dumpSequence(Array<6,T> &array);
  template <class T> void dumpSequence(Array<7,T> &array);
  template <class T> void dumpSequence(Array<8,T> &array);
  virtual void visit(String & str);
 public:
  // constructors
  DataTexter(QString fn, const char* mode);
  DataTexter(FILE * f);
  // reader & writer
  virtual void read_into(Data & target);
  virtual void write(Data data);
  // static routines
  static Data read_file(QString filename);
  static void write(Data data, QString filename);
  static void write(Data data, FILE *target);
};

//---------------------------------------------------------------
//                         Data Binner
//---------------------------------------------------------------
class DataBinner: public DataIo
{
 private:
  static const int type_data = 0;
  static const int type_array = 1;
  static const int type_token = 2;
  static const int type_symbol = 3;
  static const int type_string = 4;
  static const int type_signed1 = 5;
  static const int type_signed2 = 6;
  static const int type_signed4 = 7;
  static const int type_signed8 = 8;
  static const int type_unsigned1 = 9;
  static const int type_unsigned2 = 10;
  static const int type_unsigned4 = 11;
  static const int type_unsigned8 = 12;
  static const int type_float4 = 13;
  static const int type_float8 = 14;
  // read/write operations
  QString file_in_use;
  char * mapped_region;
  char * cur_ptr;
  unsigned4 mapped_size;
  unsigned4 file_size;
  void mmap_textfile();
  void munmap_textfile();
  void write_internal(const char* ptr, int size);
  void read_internal(char* ptr, int size);
 protected:
  virtual void write_fileformat_versionnr();
  virtual int  read_fileformat_versionnr();
  virtual void read_into(Data & target);
 public:
  DataBinner(QString fn, const char* mode);
  DataBinner(FILE *f);
  virtual ~DataBinner(); 

  virtual void start_reading();
  virtual void write(Data);
  static Data read_file(QString filename);
 public:
  static void write(Data data, QString filename);
  static void write(Data data, FILE *target);

  // actual writing
#define ARRAY_TYPE(D,T) \
  virtual void visit(Array<D,T> &array) {visitArray(array);};
  ARRAY_TYPES;
#undef ARRAY_TYPE
  template <int D, class T> void visitArray(Array<D,T> & array);
  virtual void visit(Token & token);
  virtual void visit(Symbol & symbol);
  virtual void visit(String & str);
  virtual void visit(Signed1 &nr);
  virtual void visit(Signed2 &nr);
  virtual void visit(Signed4 &nr);
  virtual void visit(Signed8 &nr);
  virtual void visit(Unsigned1 &nr);
  virtual void visit(Unsigned2 &nr);
  virtual void visit(Unsigned4 &nr);
  virtual void visit(Unsigned8 &nr);
  virtual void visit(Float4 &nr);
  virtual void visit(Float8 &nr);
 private:
  void write_typenr(int type);
  int  read_typenr();

  String read_string();
  void write_string(String s);

  Symbol read_symbol();
  void write_symbol(Symbol s);

  Signed8 read_signed8();
  Signed4 read_signed4();
  Signed2 read_signed2();
  Signed1 read_signed1();
  Unsigned8 read_unsigned8();
  Unsigned4 read_unsigned4();
  Unsigned2 read_unsigned2();
  Unsigned1 read_unsigned1();
  Float4 read_float4();
  Float8 read_float8();


  Token  read_token();
  void write_internal(Data data);
  void write_internal(signed1 s);
  void write_internal(signed2 s);
  void write_internal(signed4 s);
  void write_internal(signed8 s);
  void write_internal(unsigned1 u);
  void write_internal(unsigned2 u);
  void write_internal(unsigned4 u);
  void write_internal(unsigned8 u);
  void write_internal(float4 s);
  void write_internal(float8 s);

  void write_internal(QString s);
  QString read_internal_qstring();
  Data read_array();
  template <int D, class T> Data read_array_internal();
  void read_internal(Data &data);
  void read_internal(signed1 &s);
  void read_internal(signed2 &s);
  void read_internal(signed4 &s);
  void read_internal(signed8 &s);
  void read_internal(unsigned1 &u);
  void read_internal(unsigned2 &u);
  void read_internal(unsigned4 &u);
  void read_internal(unsigned8 &u);
  void read_internal(float4 &s);
  void read_internal(float8 &s);
  

  void write_typenr_of(Data s);
  void write_typenr_of(signed1 s);
  void write_typenr_of(signed2 s);
  void write_typenr_of(signed4 s);
  void write_typenr_of(signed8 s);
  void write_typenr_of(unsigned1 u);
  void write_typenr_of(unsigned2 u);
  void write_typenr_of(unsigned4 u);
  void write_typenr_of(unsigned8 u);
  void write_typenr_of(float4 s);
  void write_typenr_of(float8 s);
};

#endif
