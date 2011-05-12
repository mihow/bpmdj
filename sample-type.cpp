#include <stdio.h>
#include "sample-type.h"

void sample_type::write(const char* prefix, FILE * f)
{
  if (left==no_sample && right==no_sample) return;
  fprintf(f,"%s %d %d\n", prefix, left, right);
}

void sample_type::read(char* l)
{
  left = atoi(l);
  while(*l && *l!=' ') l++;
  while(*l==' ') l++;
  right = atoi(l);
}

