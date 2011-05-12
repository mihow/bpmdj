#include "../Data/data.h"
#include "../Data/array.h"
#include "../Data/array-iterator.h"

template<int D> 
void normalize(Array<D,float4> & signal, bool abs_to_unit = true)
{
  if(signal.empty()) return;
  if (abs_to_unit)
    {
      typename Array<D,float4>::values it(signal);
      float4 abs_max=fabs(*it);
      it++;
      for(; it.more(); it++)
	{
	  float4 val = fabs(*it);
	  if (val>abs_max)
	    abs_max = val;
	}
      if (abs_max==0) return;
      it.reset();
      for(; it.more(); it++)
	*it=*it/abs_max;
    }
}

