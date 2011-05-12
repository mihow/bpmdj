/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__invert_cpp__
#define __loaded__invert_cpp__
using namespace std;
#line 1 "invert.c++"
#include <cstdio>
#include <iostream>
#include <qapplication.h>
#include <qimage.h>

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  for(int i = 1 ; i < argc;i++)
    {
      QImage I= QImage(argv[i]).convertToFormat(QImage::Format_RGB32);
      if (I.isNull())
        {
	printf("Could not read %s\n",argv[i]);
        continue;
	}
      // check the average white level.
      unsigned long long average=0;
      for(int x = 0 ; x < I.width(); x++)
	for(int y = 0 ; y <I.height(); y++)
	  {
	    QRgb rgb=I.pixel(x,y);
	    average+=qRed(rgb)+qGreen(rgb)+qBlue(rgb);
	  }
      average/=3;
      average/=I.width();
      average/=I.height();
      if (average>127) // then the image is white
	{
	  cout << "Swapping color for image " << argv[i] << "\n";
	  for(int x = 0 ; x < I.width(); x++)
	    for(int y = 0 ; y <I.height(); y++)
	      {
		QRgb rgb=I.pixel(x,y);
		I.setPixel(x,y,qRgb(255-qRed(rgb),
				    255-qGreen(rgb),
				    255-qBlue(rgb)));
	      }
	  I.save(argv[i]);
	}
    }
}

#endif // __loaded__invert_cpp__
