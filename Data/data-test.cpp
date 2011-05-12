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

#include "om-data.h"

Symbol getRandomSymbol()
{
  int l = random()%5+3;
  char str[10];
  for(int i = 0 ; i < l ; i++)
    str[i]=random()%26+'a';
  str[l]=0;
  return Symbol(str);
}

String getRandomString()
{
  int l = random()%30;
  char str[100];
  for(int i = 0 ; i < l ; i++)
    str[i]=random()%26+'A';
  str[l]=0;
  return String(str);
}

Token getRandomToken()
{
  int l = random()%5+3;
  Token result;
  for(int i = 0 ; i < l ; i++)
    result[getRandomSymbol()]=getRandomString();
  return result;
}

void write_and_read_test()
{
  // create a set of various arrays
  Array<6,signed2> D6(2,2,2,2,2,2);
  int nr = 0;
  for(Array<6,signed2>::values pixel(D6); pixel.more() ; ++pixel)
    pixel=nr++;

  Array<3,unsigned1> I(3,20,10);
  for(Array<3,unsigned1>::values pixel(I); pixel.more() ; ++pixel)
    pixel=nr++;

  Array<2,Data> T(3,3);
  for(Array<2,Data>::values token(T) ; token.more() ; ++token)
    token=getRandomToken();

  Array<1,Data> L(3);
  L[0]=D6;
  L[1]=I;
  L[2]=T;

  DataTexter::write(L,stdout);
  DataBinner::write(L,"/tmp/test.dat");
  Data read = DataBinner::read("/tmp/test.dat");
  DataTexter::write(read,stdout);
}

int main(int, char*[])
{
  write_and_read_test();
  DataTexter texter("/tmp/dump.txt","ab");
  //  Data test = DataTexter::read("test.dat");
  //  texter.dump(test);
  //  return 0;

  Symbol wopper("wopper");

  Token token;
  Token beta;
  token["Alfa"]  = String("Moonbase\nalfa and 'beta' djkhe kljsdf kzejhk \nhsdfk jhsdf kjhsdfkjhzeiury ;,hkjsdhfkjhsd iuyzerkjh ukhkughiuy kiu");
  token["Delta"] = String("Blahlbah");
  token["Beta"]  = beta;
  beta.define(Symbol("wopper"),Symbol("HEY"));
  beta["Lange"]=Symbol("wapper");
  Array<1,Data> as2(2);
  as2[0]=Signed2(12);
  as2[1]=Unsigned2(19);
  token["Gamma"]=as2;
  Array<2,Data> as22(2,2);
  as22[XY(0,0)]=Signed8(10);
  as22[XY(0,1)]=Unsigned1(20);
  as22[XY(1,0)]=Signed2(30);
  as22[XY(1,1)]=Signed4(40);
  token["Kappa"]=as22;
  FILE * data = fopen("data.cpp","rb");
  fseek(data,0,SEEK_END);
  int L = ftell(data);
  fseek(data,0,SEEK_SET);
  char data_data[L];
  fread(data_data,L,1,data);
  fclose(data);
  data_data[L]=0;
  token[Symbol("data.cpp")]=String(data_data);
  token["ZZZZZZ"]=String("{Alfa : 'Moonbase\n"
	     "        >alfa and ''beta''\n"
	     "        | djkhe kljsdf kzejhk\n" 
	     "        >hsdfk jhsdf kjhsdfk\n"
	     "        |jhzeiury ;,hkjsdhfkjh\n"
	     "        |sd iuyzerkjh ukhkughi\n"
	     "        |uy kiu';\n"
	     " Beta : {Lange : wapper;\n"
	     "         Wopper : HEY};\n"
	     " Delta : 'Blahlbah';\n"
	     " Gamma : <+12w +0w +0w +0w +0w +0w +0w +0w +0w +0w +0w +0w +0w +0w +0w +19w>;\n"
	     " Kappa : <<+10b +0b +0b +0b +0b +0b +0b +0b +0b +0b +20b> <> <> <> <> <> <> <> <> <> <+30b +0b +0b +0b +0b +0b +0b +0b +0b +0b\n");
  texter.write(token);
  printf("Finished\n");
}
