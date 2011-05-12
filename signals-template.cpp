/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef SIGNALS_TEMPLATE_CPP
#define SIGNALS_TEMPLATE_CPP

#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <syscall.h>
#include <math.h>
#include "common.h"
#include "signals.h"
#include "memory.h"
#include "fourier.h"
#include "files.h"
#include <fftw3.h>
/**
 * The strategy in this library should be to put everything within a
 * signal if it can be done in place
 * otherwise we define it as functions.
 */
#define DECLARE  template <class Type, int Channels>
#define BASIC    BasicSignal<Type,Channels>
#define SIGNL    Signal<Type,Channels>
#define SAMPLE   Sample<Type,Channels>
#define SHIFT    Shift<Type,Channels>
#define DECLHAAR template <class Input, class Output, int Channels>
#define HAAR     Haar<Input,Output,Channels>

/**
 * Aux
 */
template<class type> inline type minimum(type a, type b)
{
  if (a<b) return a;
  return b;
};

/**
 * Multi channel samples
 */
DECLARE SAMPLE::Sample(Type value)
{
  for(int i = 0 ; i < Channels ; i++)
    channel[i]=value;
}

DECLARE template <class Type2>  SAMPLE::Sample(Sample<Type2,Channels> other)
{
  for(int i = 0 ; i < Channels ; i++)
    channel[i]=(Type)other[i];
}

DECLARE SAMPLE SAMPLE::operator +(SAMPLE b) const
{
  SAMPLE r;
  for(int i = 0 ; i < Channels ; i++)
    r[i]=channel[i]+b[i];
  return r;
}

DECLARE SAMPLE & SAMPLE::operator +=(SAMPLE b) 
{
  for(int i = 0 ; i < Channels ; i++)
    channel[i]+=b[i];
  return *this;
}

DECLARE SAMPLE & SAMPLE::operator -=(SAMPLE b) 
{
  for(int i = 0 ; i < Channels ; i++)
    channel[i]-=b[i];
  return *this;
}

DECLARE SAMPLE & SAMPLE::operator /=(SAMPLE b) 
{
  for(int i = 0 ; i < Channels ; i++)
    channel[i]/=b[i];
  return *this;
}

DECLARE SAMPLE & SAMPLE::operator /=(Type b) 
{
  for(int i = 0 ; i < Channels ; i++)
    channel[i]/=b;
  return *this;
}

DECLARE SAMPLE SAMPLE::operator -(SAMPLE b) const
{
  SAMPLE r;
  for(int i = 0 ; i < Channels ; i++)
    r[i]=channel[i]-b[i];
  return r;
}

DECLARE SAMPLE SAMPLE::operator *(Type b) const
{
  SAMPLE r;
  for(int i = 0 ; i < Channels ; i++)
    r[i]=channel[i]*b;
  return r;
}

DECLARE SAMPLE SAMPLE::operator /(SAMPLE b) const
{
  SAMPLE r;
  for(int i = 0 ; i < Channels ; i++)
    r[i]=channel[i] / b[i];
  return r;
}

DECLARE SAMPLE SAMPLE::operator *(SAMPLE b) const
{
  SAMPLE r;
  for(int i = 0 ; i < Channels ; i++)
    r[i]=channel[i] * b[i];
  return r;
}

DECLARE SAMPLE SAMPLE::zero_to_one() const
{
  SAMPLE r;
  for(int i = 0 ; i < Channels ; i++)
    r[i] = channel[i] == 0 ? 1 : channel[i];
  return r;
}

/**
 * Basic Signals
 */
DECLARE void BASIC::operator =(const BasicSignal<Type,Channels> &val)
{
  assert(val.length==length);
  for(int i = 0 ; i < length ; i ++)
    set(i,val[i]);
}

DECLARE SAMPLE BASIC::normalize_abs_max()
{
  SAMPLE m = find_abs_max();
  SAMPLE d = m.zero_to_one();
  printf("d = %g,%g\n",d[0],d[1]);
  divide(d);
  return m;
}

DECLARE void BASIC::set(Type v)
{
  for(unsigned4 i = 0 ; i < length; i ++)
    set(i,v);
}

DECLARE
template <class Type2> void BASIC::add(const BasicSignal<Type2,Channels> &other)
{
  unsigned4 ml = minimum(length,other.length);
  for(unsigned4 i = 0 ; i < ml ; i ++)
    set(i,get(i)+other.get(i));
}

DECLARE 
template <class Type2> BASIC& BASIC::operator -=(const BasicSignal<Type2,Channels> &other)
{
  unsigned4 ml = minimum(length,other.length);
  for(unsigned C = 0 ; C < Channels ; C++)
    for(unsigned4 i = 0 ; i < ml ; i ++)
      set(i,C,get(i,C)-other.get(i,C));
  return *this;
}

DECLARE BASIC& BASIC::operator >=(const Type &val)
{
  for(unsigned C = 0 ; C < Channels ; C++)
    for(unsigned4 i = 0 ; i < length ; i ++)
      {
	Type v  = get(i,C);
	set(i,C,v>=val ? v : val);
      }
  return *this;
}

DECLARE void BASIC::smooth(int nr)
{
  for(unsigned1 C = 0 ; C < Channels ; C++)
    {
      Type added[nr*2];
      Type sum = 0;
      for(int i = 0 ; i < nr*2 ; i++)
	sum+=added[i]=get(i,C);
      for(int i = nr ; i < length - nr ; i++)
	{
	  int ni = i + nr;
	  int nj = ni % (nr * 2);
	  sum-=added[nj];
	  sum+=added[nj]=get(ni,C);
	  set(i,C,sum/(nr*2));
	}
    }
}

DECLARE void BASIC::multiply(Type factor)
{
  for(unsigned4 i = 0 ; i < length ; i ++)
    set(i,get(i)*factor);
}

DECLARE void BASIC::divide(SAMPLE d)
{
  for(unsigned4 i = 0 ; i < length ; i ++)
    set(i,get(i)/d);
}

DECLARE void BASIC::set(unsigned4 i, Type v)
{
  SAMPLE value(v);
  set(i,value);
}

DECLARE SAMPLE BASIC::find_abs_max()
{
  if (length==0) return 0;
  SAMPLE m = get(0);
  for(unsigned1 channel = 0 ; channel < Channels ; channel++)
    {
      Type M=0;
      for(unsigned4 i = 0 ; i < length ; i ++)
	{
	  Type t = ::absolute(get(i,channel));
	  if (t>M) M=t;
	}
      m[channel]=M;
    }
  return m;
}

DECLARE void BASIC::absolute()
{
  for(unsigned1 channel = 0 ; channel < Channels ; channel++)
    for(unsigned4 i = 0 ; i < length ; i ++)
      set(i,channel,::absolute(get(i,channel)));
}

/**
 * Memory Backed Signals
 */
DECLARE void SIGNL::init_channels()
{
  owner_of_data = true;
  for(int i = 0 ; i < Channels ; i ++)
    {
      int l = BasicSignal<Type,Channels>::length;
      channel[i] = allocate(l,Type);
    }
}

DECLARE SIGNL::Signal(unsigned4 l) : BASIC(l), channel()
{
  init_channels();
}

DECLARE SIGNL::~Signal()
{
  for(int i = 0 ; i < Channels ; i++)
    {
      deallocate(channel[i]);
      channel[i]=NULL;
    }
}

DECLARE SAMPLE SIGNL::get(unsigned4 pos) const
{
  SAMPLE s;
  for(int i = 0 ; i < Channels ; i ++)
    s[i]=channel[i][pos];
  return s;
}

DECLARE Type SIGNL::get(unsigned4 pos, unsigned1 chan) const
{
  return channel[chan][pos];
}


DECLARE
template <class Type2> SIGNL::Signal(const BasicSignal<Type2,Channels> &init) :
  BASIC(init.length), channel()
{
  init_channels();
  for(unsigned4 i = 0 ; i < BASIC::length; i ++)
    set(i,init.get(i));
}

DECLARE void SIGNL::set(unsigned4 i, SAMPLE v)
{
  for(int c = 0 ; c < Channels ; c ++)
    channel[c][i]=v[c];
}

DECLARE void SIGNL::set(unsigned4 pos, unsigned1 chan, Type c)
{
  channel[chan][pos]=c;
}

/**
 * Halfcomplex storage
 */
DECLARE HalfComplex<Type,Channels>::HalfComplex(unsigned4 real, unsigned4 com) : SIGNL(real*2)
{
  assert(real==com);
  assert(real);
}

DECLARE HalfComplex<Type,Channels>& HalfComplex<Type,Channels>::operator /=(const HalfComplex<Type,Channels> &other)
{
  // this is a division of complex numbers
  for(int C = 0 ; C<Channels ; C++)
    {
      Type * zr = this->channel[C];
      Type * zi = this->channel[C]+this->length;
      Type * wr = other.channel[C];
      Type * wi = other.channel[C]+this->length;
      for(unsigned4 j = 1 ; j < this->length/2 ; j++)
	{
	  Type a = zr[j];
	  Type b = zi[-j];
	  Type c = wr[j];
	  Type d = wi[-j];
	  Type s = c*c+d*d;
	  Type e = (a*c+b*d)/s;
	  Type f = (b*c-a*d)/s;
	  zr[j]=e;
	  zi[-j]=f;
	}
      Type a = zr[0];
      Type c = wr[0];
      zr[0]=a/c;
    }
  return *this;
}

DECLARE void HalfComplex<Type,Channels>::toPolar(BasicSignal<Type,Channels> &energy)
{
  assert(energy.length == this->length/2);
  // this is a division of complex numbers
  for(int C = 0 ; C<Channels ; C++)
    {
      Type * zr = this->channel[C];
      Type * zi = this->channel[C]+this->length;
      for(unsigned4 j = 1 ; j < this->length/2 ; j++)
	{
	  Type a = zr[j];
	  Type b = zi[-j];
	  energy.set(j,C,sqrt(a*a+b*b));
	}
      energy.set(0,C,zr[0]);
    }
}

DECLARE void HalfComplex<Type,Channels>::toPolar(BasicSignal<Type,Channels> &energy, BasicSignal<Type,Channels> &angle)
{
  assert(energy.length == this->length/2);
  assert(angle.length == energy.length);
  // this is a division of complex numbers
  for(int C = 0 ; C<Channels ; C++)
    {
      Type * zr = this->channel[C];
      Type * zi = this->channel[C]+this->length;
      for(unsigned4 j = 1 ; j < this->length/2 ; j++)
	{
	  Type a = zr[j];
	  Type b = zi[-j];
	  energy.set(j,C,sqrt(a*a+b*b));
	  angle.set(j,C,atan2(b,a));
	}
      energy.set(0,C,zr[0]);
      angle.set(0,C,0);
    }
}

DECLARE void HalfComplex<Type,Channels>::fromPolar(const BasicSignal<Type,Channels> &energy, const BasicSignal<Type,Channels> &angle)
{
  assert(energy.length == this->length/2);
  assert(angle.length == energy.length);
  // this is a division of complex numbers
  for(int C = 0 ; C<Channels ; C++)
    {
      Type * zr = this->channel[C];
      Type * zi = this->channel[C]+this->length;
      for(unsigned4 j = 1 ; j < this->length/2 ; j++)
	{
	  Type alpha = angle.get(j,C);
	  Type strength = energy.get(j,C);
	  zr[j] = cos(alpha)*strength;
	  zi[-j] = sin(alpha)*strength;
	}
      zr[0]=energy.get(0,C);
    }
}


/**
 * The Shift operator
 */
DECLARE SHIFT::Shift(BASIC &b, int sh): BASIC(b.length)
{
  basis = &b;
  shift = sh;
}

DECLARE unsigned4 SHIFT::remap(unsigned4 i) const
{
  return (i+shift)%this->length;
}

DECLARE SAMPLE SHIFT::get(unsigned4 pos) const
{
  return basis->get(remap(pos));
}

DECLARE Type SHIFT::get(unsigned4 pos, unsigned1 chan) const
{
  return basis->get(remap(pos),chan);
}

DECLARE void SHIFT::set(unsigned4 pos, SAMPLE c)
{
  basis->set(remap(pos),c);
}

DECLARE void SHIFT::set(unsigned4 pos, unsigned1 chan, Type c)
{
  basis->set(remap(pos),chan,c);
}

/**
 * The FFT functionality
 */
template <int Channels> Fft<Channels>::Fft(Signal<double,Channels> &in, HalfComplex<double,Channels> &out):
  BasicConvertor(), fftw()
{
  for(int i = 0 ; i < Channels ; i++)
    {
      assert(in.channel[i]);
      assert(out.channel[i]);
      fftw[i] = fftw_plan_r2r_1d(in.length,in.channel[i],out.channel[i],
				 FFTW_R2HC,FFTW_ESTIMATE);
    }
}

template <int Channels> Fft<Channels>::Fft(HalfComplex<double,Channels> &in, Signal<double,Channels> &out):
  BasicConvertor()
{
  for(int i = 0 ; i < Channels ; i++)
    fftw[i] = fftw_plan_r2r_1d(in.length,in.channel[i],out.channel[i],
			       FFTW_HC2R,FFTW_ESTIMATE);
}

template <int Channels> void Fft<Channels>::execute()
{
  for(int i = 0 ; i < Channels ; i ++)
    fftw_execute(fftw[i]);
}

/**
 * Haar
 */

DECLHAAR HAAR::Haar(BasicSignal<Input,Channels> &i, BasicSignal<Output, Channels> &o, bool f) :
  in(i), out(o), fwd(f)
{
  assert(in.length==out.length);
  int l = out.length;
  while(l && l!=1)
    {
      assert(!(l%2));
      l>>=1;
    };
}

/*
static const Output C0 = +0.4829629131445341;
static const Output C1 = +0.8365163037378079;
static const Output C2 = +0.2241438680420134;
static const Output C3 = -0.1294095225512604;
*/


DECLHAAR void HAAR::forward(BasicSignal<Output,Channels> &a, int n)
{
static const Output C0 = +0.5;
static const Output C1 = +0.5;
static const Output C2 = +0.5;
static const Output C3 = +0.5;
  
  Signal<Output,Channels> wksp(n);
  int i,j,nh = n >> 1;
  for(i = 0, j = 0 ; j < n-3; j+=2, i++)
    {
      wksp.set(i,    a[j] * C0 + a[j+1] * C1 + a[j+2] * C2 + a[j+3] * C3);
      wksp.set(i+nh, a[j] * C3 - a[j+1] * C2 + a[j+2] * C1 - a[j+3] * C0);
    }
  wksp.set(i   , a[n-2] * C0 + a[n-1] * C1 + a[0] * C2 + a[1] * C3);
  wksp.set(i+nh, a[n-2] * C3 - a[n-1] * C2 + a[0] * C1 - a[1] * C0);
  for(i = 0 ; i < n ; i++)
    a.set(i,wksp.get(i));
}

DECLHAAR void HAAR::backward(BasicSignal<Output,Channels> &a, int n)
{
  static const Output C0 = +0.5;
  static const Output C1 = +0.5;
  static const Output C2 = +0.5;
  static const Output C3 = +0.5;
  
  Signal<Output,Channels> wksp(n);
  int i,j,nh = n >> 1;
  
  wksp.set(0,a[nh-1] * C2 + a[n-1] * C1 + a[0] * C0 + a[nh] * C3);
  wksp.set(1,a[nh-1] * C3 - a[n-1] * C0 + a[0] * C1 - a[nh] * C2);
  for(i=0, j=2 ; i < nh-1;i++)
    {
      wksp.set(j++, a[i] * C2 + a[i+nh] * C1 + a[i+1] * C0 + a[i+nh+1] * C3);
      wksp.set(j++, a[i] * C3 - a[i+nh] * C0 + a[i+1] * C1 - a[i+nh+1] * C2);
    }
  for(i = 0 ; i < n ; i++)
    a.set(i,wksp[i]);
}

DECLHAAR void HAAR::execute()
{
  int n=in.length;
  if (n<4) return;
  int i;
  for(i = 0 ;i < n ; i ++)
    out.set(i,in.get(i));
  while(i<out.length)
    out.set(i++,0);
  if (fwd)
    for(int nn=n;nn>=4;nn>>=1)
      forward(out,nn);
  else
    for(int nn=4;nn<=n;nn<<=1)
      backward(out,nn);
}

/**
 * Signal Input & Output
 */
#define SIO SignalIO<Type,Channels>
DECLARE SIO::SignalIO(FILE *to, bool w) : file(to), write(w)
{};

DECLARE SIO::SignalIO(const char * fn, const char* mode)
{
  file = fopen(fn,mode);
  write = mode[0]=='a' || mode[0]=='w';
}

DECLARE SIO::~SignalIO()
{
  if (file) fclose(file);
  file=NULL;
}

DECLARE unsigned4 SIO::samples()
{
  return fsize(file)/(Channels*sizeof(Type));
}

DECLARE template <class Type2>  void SIO::writeSamples(const BasicSignal<Type2,Channels> &s, unsigned4 write_length)
{
  assert(write);
  unsigned l = minimum(write_length,s.length);
  for(unsigned4 i = 0 ; i < l ; i ++)
    for(unsigned4 j = 0 ; j < Channels ; j++)
      {
	Type t = (Type)s.get(i,j);
	fwrite(&t,sizeof(Type),1,file);
      }
}

DECLARE template <class Type2> void SIO::readSamples(BasicSignal<Type2,Channels>& s, unsigned4 starting_at_sample)
{
  assert(file);
  int fd = fileno(file);
  const int unit = sizeof(Type);
  unsigned4 map_length = fsize(file);
  unsigned char * data = (unsigned char*)mmap(NULL,map_length,PROT_READ,MAP_SHARED,fd,0);
  assert(data!=MAP_FAILED);
  assert(data);
  assert((s.length+starting_at_sample)*Channels*unit<map_length);
  Type * D = (Type*)(data+starting_at_sample*Channels*unit);
  for(unsigned4 i = 0 ; i < s.length ; i ++)
    for(unsigned4 j = 0 ; j < Channels ; j++)
      s.set(i,j,D[i*Channels+j]);
  munmap(data,map_length);
}

/**
 * These should be removed in the future
 */
template<class type> type normalize_abs_max(type * data, long l)
{
  type m = find_abs_max(data,l);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
  return m;
}

template <class type> type find_abs_max(type * data, long l)
{
  if (l==0) return 0;
  type m = data[0];
  for(long i = 0 ; i < l ; i ++)
    if (fabs(data[i])>m)
      m=fabs(data[i]);
  return m;
}

#endif
