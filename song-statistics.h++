#include <math.h>
#include "common.h"
#include "signals.h"
#include "files.h"

extern unsigned8 total_music_body;

float4 normalize_freq(float4 in, int band);
//float4 normalize_echo(float4 val, int band, int delay);
float4 stats_get_freq(int band);

void statistics_first_pass(Song * song);
void statistics_prepare_second_pass();
void statistics_second_pass(Song * song);
void statistics_stop_second_pass();
