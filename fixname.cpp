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
#line 1 "fixname.c++"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char*argv[])
{
  printf("__BPMDJ__");
  for(int i = 1 ; i < argc; i ++)
    {
      char* s = argv[i];
      for(unsigned int j = 0 ; j < strlen(s) ; j++)
	{
	  s[j]=toupper(s[j]);
	  if (!isalnum(s[j]))
	    s[j]='_';
	}
      printf("_%s",s);
    }
}
