/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__signals_h__
#define __loaded__signals_h__
using namespace std;
#line 1 "signals.h++"
#include <fftw3.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>
#include <syscall.h>
#include <math.h>
#include "types.h"
#include "common.h"
#include "memory.h"
#include "files.h"

template <class Type, int Channels> class Sample
{
  private:
    Type channel[Channels];
  public:
    Sample() {};
    Sample(Type value);
    template <class Type2> Sample(Sample<Type2,Channels>);
    Type & operator[](unsigned1 idx) { assert(idx<Channels); return channel[idx]; };
    Sample<Type,Channels> operator +(Sample<Type,Channels> b) const;
    Sample<Type,Channels> operator -(Sample<Type,Channels> b) const;
    Sample<Type,Channels> operator *(Type b) const;
    Sample<Type,Channels> operator *(Sample<Type,Channels> b) const;
    Sample<Type,Channels> operator /(Sample<Type,Channels> b) const;
    Sample<Type,Channels> zero_to_one() const;
    Sample<Type,Channels> & operator -=(Sample<Type,Channels> b);
    Sample<Type,Channels> & operator +=(Sample<Type,Channels> b);
    Sample<Type,Channels> & operator /=(Sample<Type,Channels> b);
    Sample<Type,Channels> & operator /=(Type b);
};

/**
 * A basic signal covers all the operations necessary on a signal
 * but it does not store the data.
 */
template <class Type, int Channels> class BasicSignal
{
  public:
    unsigned4 length;
    BasicSignal(unsigned4 l) {length = l;};
    virtual ~BasicSignal() {};
    // get operations
    virtual Sample<Type,Channels> get(unsigned4 pos) const = 0;
    virtual Type get(unsigned4 pos, unsigned1 chan) const = 0;
    Sample<Type,Channels> operator[](unsigned4 idx) const { return get(idx); };
    // set operations
    void clear() { set(0); };
    virtual void set(Type c);
    virtual void set(unsigned4 pos, Type c);
    virtual void set(unsigned4 pos, unsigned1 chan, Type c) = 0;
    virtual void set(unsigned4 pos, Sample<Type,Channels> c) = 0;
    template <class Type2> void add(const BasicSignal<Type2,Channels> &other);
    template <class Type2> BasicSignal<Type,Channels>& operator -=(const BasicSignal<Type2,Channels> &other);
    BasicSignal<Type,Channels>& operator >=(const Type &val);
    void operator =(const BasicSignal<Type,Channels> &val);
    void multiply(Type m);
    void divide(Sample<Type,Channels> d);
    virtual void absolute();
    virtual void smooth(unsigned int nr);
    Sample<Type,Channels> find_abs_max();
    Sample<Type,Channels> normalize_abs_max();
};

template <int Channels> class Fft;
template <class input, class output, int Channels> class Haar;

template <class Type, int Channels> class Signal: 
  public BasicSignal<Type, Channels>
{
  private:
    void init_channels();
  protected:
    bool owner_of_data;
    Type *channel[Channels];
  public:
    Signal(unsigned4 l);
    template<class Type2> Signal(const BasicSignal<Type2,Channels> &init);
    virtual Sample<Type,Channels> get(unsigned4 pos) const;
    virtual Type get(unsigned4 pos, unsigned1 chan) const;
    virtual void set(unsigned4 pos, Sample<Type,Channels> c);
    virtual void set(unsigned4 pos, unsigned1 chan, Type c);
    friend class Fft<Channels>;
    virtual ~Signal();
};

template <class Type, int Channels> class SignalIO
{
  private:
    FILE * file;
    bool write;
  public:
    SignalIO(const char * fn, const char* mode);
    SignalIO(FILE * f, bool write = false);
    unsigned4 samples();
    template <class Type2> void writeSamples(const BasicSignal<Type2,Channels>&, unsigned4 max_length);
    template <class Type2> void readSamples(BasicSignal<Type2,Channels>&, unsigned4 starting_at_sample);
    virtual ~SignalIO();
};

/**
 * A class which offers a translated view on data and some helper
 * functions.
 */
template <class Type, int Channels> class Shift: 
  public BasicSignal<Type,Channels>
{
  private:
    BasicSignal<Type,Channels> *basis;
    int shift;
    unsigned4 remap(unsigned4 i) const;
  public:
    Shift(BasicSignal<Type,Channels> &b, int shift);
    virtual ~Shift() {};
    virtual Sample<Type,Channels> get(unsigned4 pos) const;
    virtual Type get(unsigned4 pos, unsigned1 chan) const;
    virtual void set(unsigned4 pos, Sample<Type,Channels> c);
    virtual void set(unsigned4 pos, unsigned1 chan, Type c);
};

/**
 * The halfcomplex class will use an internal representation which 
 * follows the fftw convention. The first half is the real part and from 
 * end to start in the second part we find the complex coefficients.
 */
template <class Type, int Channels> class HalfComplex: public Signal<Type, Channels>
{
  public:
    HalfComplex(unsigned4 real, unsigned4 com);
    HalfComplex<Type,Channels>& operator /=(const HalfComplex<Type,Channels> &other);
    virtual void toPolar(BasicSignal<Type,Channels> &energy);
    virtual void toPolar(BasicSignal<Type,Channels> &energy, BasicSignal<Type,Channels> &angle);
    virtual void fromPolar(const BasicSignal<Type,Channels> &energy, const BasicSignal<Type,Channels> &angle);
};

class BasicConvertor
{
  public:
    BasicConvertor();
    virtual ~BasicConvertor() {};
    virtual void execute() = 0;
};

/**
 * The FFT only works with specific signal classes
 */
template<int Channels> class Fft: public BasicConvertor
{
  private:
    fftw_plan fftw[Channels];
  public:
    Fft(Signal<double,Channels> &in, HalfComplex<double,Channels> &out); // forward
    Fft(HalfComplex<double,Channels> &in, Signal<double,Channels> &out); // backward
    void execute();
};

/**
 * The haar wavelet transform
 */
template<class Input, class Output, int Channels> class Haar: public BasicConvertor
{
  private : 
    BasicSignal<Input,Channels> &in;
    BasicSignal<Output,Channels> &out;
    bool fwd;
    void forward(BasicSignal<Output,Channels> &a, int n);
    void backward(BasicSignal<Output,Channels> &a, int n);
  public:
    Haar(BasicSignal<Input,Channels> &in, BasicSignal<Output,Channels> &out, bool forward);
    void execute();
};

/**
 * General usefull functions
 */
inline unsigned4 absolute(signed4 i) {return abs(i);};
inline unsigned2 absolute(signed2 i) {return abs(i);};
inline double    absolute(double  i) {return fabs(i);};
inline float     absolute(float   i) {return fabs(i);};

signed8 higher_power_of_two(signed8 a);
signed8 lower_power_of_two(signed8 a);
double find_max(double * data, long l);
long   find_max_rpos(unsigned4 * data, long l);
double find_mean(double * data, long l);
float  find_mean(float * data, long l);

// template <class type> type find_abs_max(type * data, long l);
// template <class type> type normalize_abs_max(type * data, long l);
void   vector_mul_div(signed4 * data, long l, signed4 mul, signed4 div);
void   normalize_mean(double * data, long l);
void   normalize_mean(float * data, long l);
void   translate_mean(float * data, long l);
double norm(double r, double e);
void   differentiate(double *r, int size);
/* sets r[i]=r[i+1]-r[i] */

void   absolute(double *r, int size);
/* sets r[i]=abs(r[i]) */

void   biased_circular_autocorrelation(double *arr, int size);
/* The biased autocorrelation will return the circular autocorrelation, or
   out[i]=sum_j in[i]*in[(j+i)%size] */

void   biased_autocorrelation(double *arr, int size);
/* The biased autocorrelation will return the non-circular autocorrelation, or
   out[i]=sum_j in[i]*in[j+i] */

void   unbiased_autocorrelation(double * arr, int size);
/* The unbiased autocorrelation will return the non-circular autocorrelation 
   taking into account the amount of actual multiplications performed.
   out[i]=autocor[i]/(size-i) */

double biased_abs_distance(double * a, double * b, int size);
double unbiased_abs_distance(double * a, double * b, int size);
/* returns |a[i]-b[i]| [ /size ] */

double biased_l2_distance(double * a, double * b, int size);
/* returns (a[i]-b[i])^2 */

double unbiased_l2_distance(double * a, double * b, int size);
/* returns biased_l2_distance / size */

double biased_best_abs_circular_distance(double *a, double *b, int size);
double biased_best_l2_circular_distance(double *a, double *b, int size);
/* finds the best circular shift to give the lowest L2 distance */

void energize(double*what, double length, unsigned4 ws);
/* will replace every element wioth the RMS value of 
 * the surrounding block of size ws */

double Index_to_frequency ( unsigned NumSamples, unsigned Index );
/* The following function returns an "abstract frequency" of a
 * given index into a buffer with a given number of frequency samples.
 * Multiply return value by sampling rate to get frequency expressed in Hz. */


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
  for(unsigned int i = 0 ; i < length ; i ++)
    set(i,val[i]);
}

DECLARE SAMPLE BASIC::normalize_abs_max()
{
  SAMPLE m = find_abs_max();
  SAMPLE d = m.zero_to_one();
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

DECLARE void BASIC::smooth(unsigned int nr)
{
  for(unsigned1 C = 0 ; C < Channels ; C++)
    {
      Type added[nr*2];
      Type sum = 0;
      for(unsigned int i = 0 ; i < nr*2 ; i++)
	sum+=added[i]=get(i,C);
      for(unsigned int i = nr ; (int)i < (int)length - (int)nr ; i++)
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
      channel[i] = bpmdj_allocate(l,Type);
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
      bpmdj_deallocate(channel[i]);
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
  unsigned int n=in.length;
  if (n<4) return;
  unsigned int i;
  for(i = 0 ;i < n ; i ++)
    out.set(i,in.get(i));
  while(i<out.length)
    out.set(i++,0);
  if (fwd)
    for(unsigned int nn=n;nn>=4;nn>>=1)
      forward(out,nn);
  else
    for(unsigned int nn=4;nn<=n;nn<<=1)
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
template<class type> 
type normalize_abs_max(type * data, long l)
{
  type m = find_abs_max(data,l);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
  return m;
}

template <class type> 
type find_abs_max(type * data, long l)
{
  if (l==0) return 0;
  type m = data[0];
  for(long i = 0 ; i < l ; i ++)
    if (fabs(data[i])>m)
      m=fabs(data[i]);
  return m;
}
#endif // __loaded__signals_h__
