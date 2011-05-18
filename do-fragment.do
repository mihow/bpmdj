#include "song.h"
#include "stereo-sample2.h"
#include "memory.h"

data FragmentInMemory
{
  stereo_sample2 *samples;
  unsigned4 size;
  FragmentInMemory();
  FragmentInMemory(QString filename);
};

data FragmentFile
{
  QString f;
  Song* song;
  bool isEmpty();
  FragmentFile();
  FragmentFile(Song *s, QString f);
  FragmentInMemory getFragment();
};
