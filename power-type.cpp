#include <stdio.h>
#include <stdlib.h>
#include "power-type.h"

void power_type::write(const char* prefix, FILE * f)
{
  if (left==no_power && right==no_power) return;
  fprintf(f,"%s %g %g\n", prefix, left, right);
}

void power_type::read(char* l)
{
  left = atof(l);
  while(*l && *l!=' ') l++;
  while(*l==' ') l++;
  right = atof(l);
}

