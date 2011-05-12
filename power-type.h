const float no_power = -1.0;
class power_type
{
 public:
  float left;
  float right;
  bool fully_defined() {return left!=no_power && right!=no_power;};
  power_type()
    {
      right = left = no_power;
    }
  power_type(float l, float r)
    {
      left = l;
      right = r;
    }
  void write(const char*prefix, FILE * output);
  void read(char*);
};

