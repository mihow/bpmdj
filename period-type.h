class period_type
{
 public:
  int period;
  static const int no_period = -1;
  period_type()
    {
      period = no_period;
    }
  period_type(int p)
    {
      period = p;
    }
  const bool none()
    {
      return period == no_period;
    }
  const bool valid()
    {
      return period > 0;
    }
  const period_type doubled()
    {
      return period_type(period*2);
    }
  const period_type halved()
    {
      return period_type(period/2);
    }
  const void write_idx(FILE * f)
    {
      fprintf(f,"period   : %d\n",period);
    }
  const void write_bib_v261(FILE * index)
    {
      file_long(period,index);
    }
  const period_type lower(int delta)
    {
      if (period-delta < 100)
	return period_type(100);
      return period_type(period-delta);
    }
  const period_type higher(int delta)
    {
      return period_type(period+delta);
    }
};

class quad_period_type
{
 public:
  signed8 qperiod;
  static const int no_period = -1;
  quad_period_type()
    {
      qperiod = no_period;
    }
  quad_period_type(int p)
    {
      qperiod = p;
    }
  const bool none()
    {
      return qperiod == no_period;
    }
  const bool valid()
    {
      return qperiod > 0;
    }
  const quad_period_type doubled()
    {
      return quad_period_type(qperiod*2);
    }
  const quad_period_type halved()
    {
      return quad_period_type(qperiod/2);
    }
  const quad_period_type muldiv(signed8 mul, signed8 div)
    {
      return quad_period_type(qperiod*mul/div);
    }
  operator signed8()
    {
      return qperiod;
    }
};
