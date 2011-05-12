#ifndef ARRAY_CREATOR_H
#define ARRAY_CREATOR_H
#include "array.h"
#include "numbers.h"

//===============================================================
//                    Array Creation 
//===============================================================
class NoElement{};

// this class keeps the next index in the first index
// the actual size can be extended through 
class Creator
{
  typedef Array<1,Data> Content;
  Content content;
  void init();
  void ensure_size(int S);
 private:
  template<int D, class T> Data createAndFillTarget(Size<8> s);
    void updateSize(Size<8>& size, int last_depth, int depth = 0 );
 public:
  Creator() : content(1) { content[0]=Unsigned4(1); };
  Creator(Data data) : content(Array<1,Data>(data)) {};
  operator Data() {return content;};
  Data convertTo(Unsigned4 dimension, Symbol type);
  void append(Data& e);
};

#endif
