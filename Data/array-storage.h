#ifndef ARRAY_STORAGE_H
#define ARRAY_STORAGE_H
//---------------------------------------------------------------
//                          Array storage
//---------------------------------------------------------------
template <class T> class ArrayStorage
{
 public:
  int  refcount;            // the number of times this data chunk is accessed
  T   *data;                // the actual data
  ArrayStorage()
    {
      refcount = 1;
      data = NULL;
    }
  ~ArrayStorage()
    {
      delete[] data;
      data = NULL;
    }
  void allocate(int size)
    {
      assert(!data);
      data = new T[size];
      //  printf("Allocated %d bytes\n",sizeof(T)*size);
    }
};
#endif

