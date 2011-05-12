/****
 Borg4 Data Library
 Copyright (C) 2005-2009 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__data.html

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
#ifndef __loaded__data_io_cpp__
#define __loaded__data_io_cpp__
using namespace std;
#line 1 "data-io.c++"
#include <sys/mman.h>
#include <qstring.h>
#include "symbol.h"
#include "array.h"
#include "array-iterator.h"
#include "data-token.h"
#include "null.h"
#include "data-string.h"
#include "data-io.h"
#include "version.h"

//---------------------------------------------------------------
//                            DataIo
//---------------------------------------------------------------
DataIo::DataIo(QString qfn, const char * mode)
{
  const char *fn;
#if QT_VERSION > 0x040000
  QByteArray F = qfn.toAscii();
  fn=F.data();
#else
  fn=qfn.ascii();
#endif
  text = fopen(fn,mode);
  opened_file_myself = true;
  if (!text)
    {
      printf("Could not open file \"%s\"\n",fn);
      assert(0);
      exit(1);
    }
  version = DATA_VERSION_NR;
}

DataIo::DataIo(FILE * f)
{
  text = f;
  opened_file_myself = false;
  assert(text);
  version = DATA_VERSION_NR;
}

DataIo::~DataIo()
{
  if (text && opened_file_myself) 
    fclose(text);
};

void DataIo::visit(Data &data)
{
  DataVisitor::visit(data);
};

void DataIo::start_writing()
{
  write_fileformat_versionnr();
}

void DataIo::start_reading()
{
  version = read_fileformat_versionnr();
}

Data DataIo::read()
{
  Data result;
  read_into(result);
  return result;  
}

//---------------------------------------------------------------
//                            DataTexter
//---------------------------------------------------------------
template <int D, class T>
void DataTexter::visitArray(Array<D,T> & array)
{
  if (type_name(T())=="Data" && D==1)
    {
      if (D>=1) dumpSequence(array);
    }
  else
    {
      int mark = push("<"+QString::number(D)+":");
      padded(type_name(T())+">\n");
      if (D>=1) dumpSequence(array);
      pop(mark,"");
    }
};

template <class T>
void DataTexter::dumpSequence(Array<1,T> & array)
{
  int mark = push("[");
  int count = array.size(0);
  for(typename Array<1,T>::positions ei(array); ei.more() ; ++ei)
    {
      int lastline = get_line();
      Data data = to_data(*ei);
      DataVisitor::visit(data);
      if (ei[0]!=count-1)
	{
	  padded(" ");
	  if (get_line() > lastline && !newline_waiting())
	    padded("\n");
	}
    }
  pop(mark,"]");
};

#define DUMPER(D) template <class T> \
void DataTexter::dumpSequence(Array<D,T> & array) \
{ \
  int mark = push("["); \
  int count = array.size(0); \
  for(ArrayIterator<D,T,true,D-1,false> submatrix(array,0) ; submatrix.more() ; ++submatrix) \
    {\
	int lastline = get_line();\
	Array<D-1,T> &sub  = submatrix;\
	dumpSequence(sub);\
	if (submatrix[0]!=count-1)\
	  {\
	    if (D==2) padded("\n"); else padded(" ");\
	    if (get_line() > lastline && !newline_waiting())\
	      padded("\n");\
	  }\
      }\
  pop(mark,"]");\
};

DUMPER(2)
DUMPER(3)
DUMPER(4)
DUMPER(5)
DUMPER(6)
DUMPER(7)
DUMPER(8)
#undef DUMP

void DataTexter::visit(Symbol & symbol) 
{
  padded(symbol());
};

void DataTexter::visit(Token & token) 
{
  int mark = push("{");
  // find the maximum length
  vector<Symbol> keys = token.keys();
  int maximumlength = 0;
  for(unsigned int i = 0 ; i < keys.size() ; i ++)
    {
      int l = keys[i].length();
      if (l>maximumlength)
	maximumlength = l;
    }
  
  // print out the bindings
  bool first = true;
  int lastline = 0;
  for(unsigned int i = 0 ; i < keys.size() ; i++)
    {
      if (!first)
	{
	  //	  padded(";\n");
	  padded("\n");
	  if (get_line() > lastline && !newline_waiting())
	    padded("\n");
	}
      else first = false;
      
      Symbol symbol = keys[i];
      Data   value = token[symbol];
      lastline = get_line();
      { 
	int mark = push("");
	int lastline = get_line();
	visit(symbol);
	int kl = symbol.length();
	while(kl<maximumlength)
	  {
	    padded(" ");
	    kl++;
	  }
	padded(" : ");
	if (get_line() > lastline && !newline_waiting())
	  padded("\n");
	DataIo::visit(value);
	pop(mark,"");
      }
    }
  pop(mark,"}");
}

void DataTexter::visit(String & str)
{
  int linelength = 80;
  int wordsplitboundary = 70;
  // do we have newlines in the string
#if QT_VERSION > 0x040000
  QByteArray utf8 = str().toUtf8();
  const char *text=utf8;
  int sl = utf8.length();
#else
  QCString utf8 = str().utf8();
  const char *text=utf8;
  int sl = utf8.length();
#endif
  // allocate space for splitting points
  int psp = 1+(sl/wordsplitboundary);
  int splitat[psp];
  int nextsplitpos = 0;
  for(int i = 0 ; i < psp ; i++)
    splitat[i]=sl+1;
  // calculate splitting points
  int curlen = 0;
  int lastbreak = 0;
  int laststart = 0;
  bool lastbreak_canbeset = true;
  for(int i = 0 ; i < sl ; i++)
    {
      char c = text[i];
      if (c=='\n')
	{
	  curlen = 0;
	  laststart = i+1;
	  lastbreak = i+1;
	}
      if (c==' ' || c=='\t')
	{
	  if (lastbreak_canbeset)
	    {
	      lastbreak = i;
	      lastbreak_canbeset=false;
	    }
	}
      else lastbreak_canbeset=true;
      if (curlen>=linelength)
	{
	  if (lastbreak-laststart > wordsplitboundary)
	    {
	      splitat[nextsplitpos++]=lastbreak;
	      laststart = lastbreak;
	      curlen = i - lastbreak;
	    }
	  else
	    {
	      splitat[nextsplitpos++]=i++;
	      laststart = i+1;
	      lastbreak = i+1;
	      curlen = 0;
	    }
	}
      curlen++;
    }
  // go through line to print this time
  int mark = get_margin();
  set_margin(get_pos());
  padded("'");
  nextsplitpos=0;
  for(int i = 0 ; i < sl ; i++)
    {
      char c = text[i];
      // do we need a break before printing this character ?
      if (i==splitat[nextsplitpos])
	{
	  padded("\n|");
	  nextsplitpos++;
	}
      // do we have a newline
      if (c=='\n')
	{
	  padded("\n>");
	  continue;
	}
      // 
      else
	{
	  if (c=='\'')
	    padded("''");
	  else
	    {
	      char c2[2];
	      c2[0]=c;
	      c2[1]=0;
	      padded(c2);
	    }
	}
    }
  pop(mark,"\'");
}

DataTexter::DataTexter(QString fn, const char* mode) : DataIo(fn,mode), newline(true)
{
  lines = 0;
  written = 0;
  margin = 0;
}

DataTexter::DataTexter(FILE * f) : DataIo(f), newline(true)
{
  lines = 0;
  written = 0;
  margin = 0;
}

void DataTexter::padded(QString str)
{
#if QT_VERSION > 0x040000
  padded((const char*)str.toAscii());
#else
  padded(str.ascii());
#endif
}

void DataTexter::padded(const char *t)
{
  // if it is a new line, pad the length up to margin
  int text_length = strlen(t);
  while(text_length>0)
    {
      char a = t[0];
      if (a=='\n')
	{
	  newline=true;
	  written=margin;
	  fprintf(text,"\n");
	  lines++;
	}
      else
	{
	  if (newline)
	    for(int i = 0; i < margin ; i ++)
	      fprintf(text," ");
	  fprintf(text,"%c",a);
	  written++;
	  newline=false;
	}
      t++;
      text_length--;
    }
}

int DataTexter::push(const char* t)
{
  padded(t);
  int old_margin = get_margin();
  set_margin(get_pos());
  return old_margin;
}

int DataTexter::push(QString str)
{
#if QT_VERSION > 0x040000
  QByteArray cs = str.toUtf8();
#else
  QCString cs = str.utf8();
#endif
  return push((const char*)cs);
}

int DataTexter::push(const char t)
{
  char c2[2];
  c2[0]=t;
  c2[1]=0;
  return push(c2);
}

void DataTexter::pop(int om, const char t)
{
  char c2[2];
  c2[0]=t;
  c2[1]=0;
  pop(om,c2);
}

void DataTexter::pop(int old_margin, const char* t)
{
  set_margin(old_margin);
  padded(t);
};

void DataTexter::visit(Float4 & f4)
{
  char temp[500];
  sprintf(temp,"%gf",(float4)f4);
  padded(temp);
}

void DataTexter::visit(Float8 & f8)
{
  char temp[500];
  sprintf(temp,"%gg",(float8)f8);
  padded(temp);
}

void DataTexter::visitSignedNumber(signed long long a, char sig) 
{
  char temp[500];
  if (a>=0)
    sprintf(temp,"+%lld%c",a,sig);
  else
    sprintf(temp,"-%lld%c",-a,sig);
  padded(temp);
}

void DataTexter::visitUnsignedNumber(unsigned long long a, char sig) 
{
  char temp[500];
  sprintf(temp,"%lld%c",a,sig);
  padded(temp);
}

void yyset_in(FILE * i);
extern Data parse_result;
int yyparse();

void DataTexter::read_into(Data& result)
{
  if (!text) 
    {
      result = Null();
      return;
    }
  static bool busy_reading = false;
  assert(!busy_reading);
  busy_reading = true;
  yyset_in(text);
  parse_result = Data();
  yyparse();
  busy_reading=false;
  result = parse_result;
}

Data DataTexter::read_file(QString filename)
{
  DataTexter texter(filename,"rb");
  texter.start_reading();
  return texter.read();
}

Data DataTexter::read_file(FILE * f)
{
   DataTexter texter(f);
   texter.start_reading();
   return texter.read();
}

void DataTexter::write(Data data, QString filename)
{
  DataTexter dt(filename,"wb");
  dt.start_writing();
  dt.write(data);
  dt.padded("\n");
}

void DataTexter::write(Data data, FILE *file)
{
  DataTexter dt(file);
  dt.start_writing();
  dt.write(data);
  dt.padded("\n");
}

void DataTexter::write(Data data)
{
  DataIo::visit(data);
};

void DataTexter::write_fileformat_versionnr()
{
  // we write out all the hex numbers readable
  fprintf(text,"0x%08x\n",version);
}

int DataTexter::read_fileformat_versionnr()
{
  int version;
  // we read the characters one by one
  fscanf(text,"0x%08x\n",&version);
  return version;
}

//===============================================================
//                          Data Binner
//===============================================================
DataBinner::DataBinner(QString fn, const char* mode) : DataIo(fn,mode) 
{
  file_in_use=fn;
  mapped_region = NULL;
  mapped_size = 0;
};

DataBinner::DataBinner(FILE *f) : DataIo(f) 
{
  file_in_use="<unknown>";
  mapped_region = NULL;
  mapped_size = 0;
};

DataBinner::~DataBinner()
{
  munmap_textfile();
}

void DataBinner::write(Data data, QString filename)
{
  DataBinner dt(filename,"wb");
  dt.start_writing();
  dt.write(data);
}

void DataBinner::write(Data data, FILE *file)
{
  DataBinner dt(file);
  dt.start_writing();
  dt.write(data);
}

Data DataBinner::read_file(QString filename)
{
  DataBinner binner(filename,"rb");
  binner.start_reading();
  return binner.read();
}

void DataBinner::write_fileformat_versionnr()
{
  assert(sizeof(version)==4);
  size_t w = fwrite(&version,4,1,text);
  assert(w = 1);
}

int DataBinner::read_fileformat_versionnr()
{
  if (cur_ptr+4>mapped_region+file_size)
    {
#if QT_VERSION > 0x040000

      QByteArray ba=file_in_use.toAscii();
#else
      QByteArray ba=file_in_use.ascii();
#endif
      printf("Could not read fileformat version from file %s\n",(const char*)ba);
      exit(150);
    }
  int version=*(int*)cur_ptr;
  cur_ptr+=4;
  assert(sizeof(version)==4);
  return version;
}

//---------------------------------------------------------------
//                 Memory Mapping for file reading
//---------------------------------------------------------------
void DataBinner::start_reading()
{
  mmap_textfile();
  DataIo::start_reading();
}

void DataBinner::mmap_textfile()
{
#ifdef LINUX
  fseek(text,0,SEEK_END);
  file_size =  mapped_size = ftell(text);
  fseek(text,0,SEEK_SET);
  fileno(text);
  //  long pagesize = sysconf(_SC_PAGESIZE);
  int pagesize = getpagesize();
  mapped_size/=pagesize;
  mapped_size++;
  mapped_size*=pagesize;
  mapped_region = (char*)mmap(NULL, mapped_size, 
			      PROT_READ, 
			      MAP_PRIVATE | MAP_POPULATE, 
			      fileno(text), 0);
  cur_ptr = mapped_region;
  assert(mapped_region);
  assert(mapped_region != MAP_FAILED);
#endif

#ifdef WINDOWS
  /**
   * I have no idea how to mmap files in windows so we stick to
   * copying the entire file in memory
   */
  fseek(text,0,SEEK_END);
  file_size = mapped_size = ftell(text);
  fseek(text,0,SEEK_SET);
  mapped_region = (char*)malloc(mapped_size);
  assert(mapped_region);
  //  long pagesize = sysconf(_SC_PAGESIZE);
  int pagesize = 4096;
  long int remaining  = mapped_size;
  char* ptr = mapped_region;
  while(remaining)
    {
      int toread = remaining;
      if (toread>pagesize) toread=pagesize;
      int read = fread(ptr,pagesize,1,text);
      assert(read==toread);
      remaining-=read;
      ptr+=read;
    }
  cur_ptr = mapped_region;
#endif
  
}

void DataBinner::munmap_textfile()
{
#ifdef LINUX
  munmap(mapped_region,mapped_size);
#endif

#ifdef WINDOWS
  if (mapped_region)
    free(mapped_region);
#endif
}

void DataBinner::write_internal(const char * ptre, int size)
{
  size_t w = fwrite(ptre,size,1,text);
  assert(w = 1);
};

void DataBinner::read_internal(char * ptre, int size)
{
  if (cur_ptr+size>mapped_region+file_size)
    {
#if QT_VERSION > 0x040000
      QByteArray ba=file_in_use.toAscii();
#else
      QByteArray ba=file_in_use.ascii();
#endif
      printf("Could not read full data from file %s\n",(const char*)ba);
      exit(150);
    }
  memcpy(ptre,cur_ptr,size);
  cur_ptr+=size;
};

//---------------------------------------------------------------
//                            Array Binner
//---------------------------------------------------------------
template <int D, class T>
void DataBinner::visitArray(Array<D,T> & array)
{
  write_typenr(type_array);     // A
  write_internal((unsigned1)D); // B
  write_typenr_of(T());         // C
  for(int d = 0 ; d < D ; d++)  
    write_internal((unsigned8)array.size(d));  // D
  for(typename Array<D,T>::ordered element(array) ; element.more() ; ++element)

    write_internal((T&)element);  // E
};

Data DataBinner::read_array()
{
  unsigned1 dimension;
  read_internal(dimension);         // B
  int T = read_typenr();            // C
  const char * type = NULL;
  switch(T)
    {
    case type_data : type = "Data"; break;
    case type_signed1 : type = "signed1"; break;
    case type_signed2 : type = "signed2"; break;
    case type_signed4 : type = "signed4"; break;
    case type_signed8 : type = "signed8"; break;
    case type_unsigned1 : type = "unsigned1"; break;
    case type_unsigned2 : type = "unsigned2"; break;
    case type_unsigned4 : type = "unsigned4"; break;
    case type_unsigned8 : type = "unsigned8"; break;
    case type_float4 : type = "float4"; break;
    case type_float8 : type = "float8"; break;
    default: 
      printf("Unknown type %d in DataBinner::read_array\n",T);
      assert(0);
    }
#define ARRAY_TYPE(D,T) \
if (D==dimension && strcmp(type,#T)==0) return read_array_internal<D,T>();
  ARRAY_TYPES
#undef ARRAY_TYPE
  assert(0);
}

template <int D, class T> 
Data DataBinner::read_array_internal()
{
  Size<D> size;
  unsigned8 dim_length;
  for(int i = 0 ; i < D ; i++)
    {
      read_internal(dim_length);   //D
      size[i] = dim_length;
    }
  //  size.print();
  Array<D,T> result(size);
  typename Array<D,T>::ordered element(result);
  // if the array linear and 'mass' - copyable ?
  if (element.linear() && typeid(T)!=typeid(Data))
    while(element.more())
      {
	read_internal((char*)element.current(),element.size()*sizeof(T));  // E's
	element.skipBlock();
      }
  else
    for(typename Array<D,T>::ordered element(result) ; element.more() ; ++element)
      read_internal((T&)element);  // one E
  return result;
}

//---------------------------------------------------------------
//                      Polymorphic Data Binner
//---------------------------------------------------------------
void DataBinner::write_internal(Data data)
{
  write(data);   // A
};

void DataBinner::write_internal(String data)
{
  write(data);   // A
};

void DataBinner::read_internal(Data &data)
{
  read_into(data); // B
}

void DataBinner::read_internal(String &data)
{    
   data=read_string(); // B
}

void DataBinner::read_into(Data & result)
{
  // in order to read some data we must first get the type specification
  int type_nr = read_typenr();
  switch(type_nr)
    {
    case type_array: result =  read_array(); return;
    case type_symbol: result =read_symbol();return;
    case type_string: result =read_string();return;
    case type_token: result =read_token();return;
    case type_signed8: result =read_signed8();return;
    case type_signed4: result =read_signed4();return;
    case type_signed2: result =read_signed2();return;
    case type_signed1: result =read_signed1();return;
    case type_unsigned8: result =read_unsigned8();return;
    case type_unsigned4: result =read_unsigned4();return;
    case type_unsigned2: result =read_unsigned2();return;
    case type_unsigned1: result =read_unsigned1();return;
    case type_float4: result =read_float4();return;
    case type_float8: result =read_float8();return;
    }
  printf("Unknown type number %d\n",type_nr);
  assert(0);
}

void DataBinner::write(Data data)
{
  DataIo::visit(data);
}

//---------------------------------------------------------------
//                      Type information
//---------------------------------------------------------------
void DataBinner::write_typenr(int type)
{
  // printf("Writing type nr %d\n",type);
  write_internal((unsigned1)type); // A
};

int DataBinner::read_typenr()
{
  unsigned1 t;
  read_internal(t);  // A
  // printf("Reading type nr %d\n",t);
  return t;
}

void DataBinner::write_typenr_of(Data)
{
  write_typenr(type_data); 
};

void DataBinner::write_typenr_of(signed1)
{
  write_typenr(type_signed1); 
};

void DataBinner::write_typenr_of(signed2)
{
  write_typenr(type_signed2);
};

void DataBinner::write_typenr_of(signed4)
{
  write_typenr(type_signed4); 
};

void DataBinner::write_typenr_of(signed8)
{
  write_typenr(type_signed8);
};

void DataBinner::write_typenr_of(unsigned1)
{ 
  write_typenr(type_unsigned1); 
};

void DataBinner::write_typenr_of(unsigned2)
{
  write_typenr(type_unsigned2); 
};

void DataBinner::write_typenr_of(unsigned4)
{
  write_typenr(type_unsigned4); 
};

void DataBinner::write_typenr_of(unsigned8)
{
  write_typenr(type_unsigned8); 
};

void DataBinner::write_typenr_of(float4)
{
  write_typenr(type_float4); 
};

void DataBinner::write_typenr_of(float8)
{
  write_typenr(type_float8); 
};

//---------------------------------------------------------------
//               Actual Data Reading / Writing
//---------------------------------------------------------------
void DataBinner::write_internal(signed1 s)
{
  write_internal((char*)&s,1);
};

void DataBinner::write_internal(signed2 s)
{
  write_internal((char*)&s,2);
};

void DataBinner::write_internal(signed4 s)
{
  write_internal((char*)&s,4);
};

void DataBinner::write_internal(signed8 s)
{
  write_internal((char*)&s,8);
};

void DataBinner::write_internal(float4 s)
{
  write_internal((char*)&s,4);
};

void DataBinner::write_internal(float8 s)
{
  write_internal((char*)&s,8);
};

void DataBinner::write_internal(unsigned1 u)
{
  write_internal((char*)&u,1);
};

void DataBinner::write_internal(unsigned2 u)
{
  write_internal((char*)&u,2);
};

void DataBinner::write_internal(unsigned4 u)
{
  write_internal((char*)&u,4);
};

void DataBinner::write_internal(unsigned8 u)
{
  write_internal((char*)&u,8);
};

#define READ_INTERNAL(TYPE) \
void DataBinner::read_internal(TYPE &s) \
{\
  s = *(TYPE*)cur_ptr;\
  cur_ptr+=sizeof(TYPE);\
};

READ_INTERNAL(signed1);
READ_INTERNAL(signed2);
READ_INTERNAL(signed4);
READ_INTERNAL(signed8);
READ_INTERNAL(unsigned1);
READ_INTERNAL(unsigned2);
READ_INTERNAL(unsigned4);
READ_INTERNAL(unsigned8);
READ_INTERNAL(float4);
READ_INTERNAL(float8);

//---------------------------------------------------------------
//                         Token Binner
//---------------------------------------------------------------
void DataBinner::visit(Token & token) 
{
  write_typenr(type_token);
  // find the maximum length
  vector<Symbol> keys = token.keys();
  unsigned4 length = keys.size();           // A
  write_internal(length);
  for(unsigned int i = 0 ; i < length ; i++)
    {
      Symbol symbol = keys[i];        
      write_symbol(symbol);           // B
      Data   value = token[symbol];
      write_internal(value);          // C
    }
}

Token DataBinner::read_token()
{
  Token result;
  unsigned4 length;
  read_internal(length);              // A
  while(length--)
    {
      Symbol symbol = read_symbol();  // B
      read_internal(result[symbol]);  // C
    }
  return result;
}

//---------------------------------------------------------------
//                         Symbol Binner
//---------------------------------------------------------------
void DataBinner::visit(Symbol & symbol)
{
  write_typenr(type_symbol);
  write_symbol(symbol);
}

void DataBinner::write_symbol(Symbol s)
{
  write_internal(s());
}

Symbol DataBinner::read_symbol()
{
  return Symbol(read_internal_qstring());
}

//---------------------------------------------------------------
//                         Number Binners
//---------------------------------------------------------------
void DataBinner::visit(Signed1 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Unsigned1 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Signed2 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Unsigned2 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Signed4 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Unsigned4 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Signed8 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Unsigned8 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

Signed8 DataBinner::read_signed8()
{
  signed8 s;
  read_internal(s);
  return Signed8(s);
}

Signed4 DataBinner::read_signed4()
{
  signed4 s;
  read_internal(s);
  return Signed4(s);
}

Signed2 DataBinner::read_signed2()
{
  signed2 s;
  read_internal(s);
  return Signed2(s);
}

Signed1 DataBinner::read_signed1()
{
  signed1 s;
  read_internal(s);
  return Signed1(s);
}

Unsigned8 DataBinner::read_unsigned8()
{
  unsigned8 s;
  read_internal(s);
  return Unsigned8(s);
}

Unsigned4 DataBinner::read_unsigned4()
{
  unsigned4 s;
  read_internal(s);
  return Unsigned4(s);
}

Unsigned2 DataBinner::read_unsigned2()
{
  unsigned2 s;
  read_internal(s);
  return Unsigned2(s);
}

Unsigned1 DataBinner::read_unsigned1()
{
  unsigned1 s;
  read_internal(s);
  return Unsigned1(s);
}

void DataBinner::visit(Float4 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

void DataBinner::visit(Float8 &nr)
{
  write_typenr_of(nr());
  write_internal(nr());
}

Float4 DataBinner::read_float4()
{
  float4 s;
  read_internal(s);
  return Float4(s);
}

Float8 DataBinner::read_float8()
{
  float8 s;
  read_internal(s);
  return Float8(s);
}


//---------------------------------------------------------------
//                         String Binner
//---------------------------------------------------------------
void DataBinner::visit(String & s)
{
  write_typenr(type_string);
  write_string(s);
}

void DataBinner::write_string(String s)
{
  write_internal(s());
}

String DataBinner::read_string()
{
  return read_internal_qstring();
}

void DataBinner::write_internal(QString s)
{
#if QT_VERSION > 0x040000
  QByteArray cs = s.toUtf8();
#else
  QCString cs = s.utf8();
#endif
  const char * T = cs;
  unsigned4 l = strlen(T);
  write_internal(l);             // A
  write_internal(T,l+1);         // B
}

QString DataBinner::read_internal_qstring()
{
  unsigned4 l;
  read_internal(l);             // A
  char str[l+1]; 
  read_internal(str,l+1);       // B
  return QString::fromUtf8(strdup(str));
}
#endif // __loaded__data_io_cpp__
