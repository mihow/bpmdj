/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__spectrum_analyzer_h__
#define __loaded__spectrum_analyzer_h__
using namespace std;
#line 1 "spectrum-analyzer.h++"
#include "ui-spectrum.h"
#include "analyzer.h"

class SpectrumAnalyzer : 
  public QWidget, 
  public Ui::SpectrumAnalyzerForm, 
  public Analyzer
{
  Q_OBJECT
public:
  SpectrumAnalyzer(QWidget*parent=0);
public slots:
  virtual void fetchSpectrum_normal();
  virtual void fetchSpectrum_wavelet();
  virtual void analyze() { fetchSpectrum_normal(); };
  virtual void on_fetchSpectrum_clicked() {start(); };
};
#endif // __loaded__spectrum_analyzer_h__
