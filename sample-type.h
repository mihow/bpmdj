#include <stdlib.h>
#include "common.h"

class sample_type
{
  public:
  static const signed4 no_sample=0xffff+1; // one more than the possible maximum of a signed short
  signed4 left;
  signed4 right;
  sample_type()
    {
      left = no_sample;
      right = no_sample;
    }
  sample_type(signed4 l, signed4 r)
    {
      left = l;
      right = r;
    }
  bool fully_defined() {return left!=no_sample && right!=no_sample;};
  void write(const char*prefix, FILE * output);
  void read(char*);
};

