#ifndef SOUND_DRIVER_H
#define SOUND_DRIVER_H

#include <stdint.h>

void sd_beep(uint32_t freq, uint64_t time);
void sd_play_music(uint32_t* freqs, uint64_t* durations, uint64_t count);
void sd_music_step();

#endif