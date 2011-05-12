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

#ifndef SIGNALS_H
#define SIGNALS_H

#include <fftw3.h>
#include <stdlib.h>
#include "basic-types.h"

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
    virtual void smooth(int nr);
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
double find_max(double * data, long l);
long   find_max_rpos(unsigned4 * data, long l);
double find_mean(double * data, long l);
float  find_mean(float * data, long l);

template <class type> type find_abs_max(type * data, long l);
template <class type> type normalize_abs_max(type * data, long l);
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

void   test_signals();

#endif
