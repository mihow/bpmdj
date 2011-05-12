#include "tempo-type.h"
#include "period-type.h"
#include "sample-type.h"
#include "power-type.h"

// period_type      - describes the length of a period in the index file, normalized to 44100 Hz
// quad_period_type - describes the length of a measure in the player, depending on different wavrates
// tempo_type       - describes the BPM of the song, given a period_type

inline tempo_type period_to_tempo(period_type a)
{
  tempo_type b;
  if (a.period > 0)
    b.tempo = 4.0*11025.0*60.0/(double)a.period;
  return b;
}

inline quad_period_type period_to_quad(period_type a)
{
  return quad_period_type(a.period * 4);
}

inline tempo_type between_tempos(tempo_type a, tempo_type b, float percent)
{
  return tempo_type((b.tempo-a.tempo)*percent+a.tempo);
}

inline int compare_tempo(tempo_type a, tempo_type b)
{
  if (a.tempo > b.tempo) return 1;
  if (a.tempo < b.tempo) return -1;
  return 0;
}
