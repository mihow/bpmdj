/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include "common.h"
class QString;
extern spectrum_freq scales[spectrum_size];
// multiplying a frequency band i with scale[i] normalizes the band to
// its mean value. E.g a frequence which is the mean will become 1 afterward.
// a frequency which is higher than mean will have a value larger than 1
// and a frequence with a value lower than mean will become lesser than 1
void new_spectrum(spectrum_type spectrum);
void last_spectrum();
