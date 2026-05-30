#ifndef PACMAN_AUDIO_H
#define PACMAN_AUDIO_H

#include <stdint.h>

#define NOTE_B4  494
#define NOTE_B5  988
#define NOTE_FS5 740
#define NOTE_DS5 622
#define NOTE_C5  523
#define NOTE_C6  1047
#define NOTE_G5  784
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_E6 1319

/*
 * 18 ticks = 1 segundo
 *
 * 1 tick ≈ 55 ms
 */

const uint32_t melodyFrequencies[] = {
    NOTE_B4,
    NOTE_B5,
    NOTE_FS5,
    NOTE_DS5,

    NOTE_B5,
    NOTE_FS5,
    NOTE_DS5,
    NOTE_C5,

    NOTE_C6,
    NOTE_G5,
    NOTE_E5,
    NOTE_C6,

    NOTE_G5,
    NOTE_E5,
    NOTE_B4,
    NOTE_B5,

    NOTE_FS5,
    NOTE_DS5,
    NOTE_B5,
    NOTE_FS5,

    NOTE_DS5,
    NOTE_DS5,
    NOTE_E5,
    NOTE_F5,

    NOTE_F5,
    NOTE_FS5,
    NOTE_G5,
    NOTE_G5,

    NOTE_GS5,
    NOTE_A5,
    NOTE_B5
};

const uint64_t melodyDurations[] = {
    2,2,2,2,
    1,2,3,2,

    2,2,2,1,
    2,3,2,2,

    2,2,1,2,
    2,1,1,1,

    1,1,2,1,
    1,1,4
};

#define MELODY_NOTE_COUNT sizeof(melodyFrequencies) / sizeof(melodyFrequencies[0])

const uint32_t pelletFrequencies[] = {
    NOTE_B5,
    NOTE_E6
};

const uint32_t pelletDurations[] = {
    1, 1
};

#define PELLET_NOTE_COUNT 2;

#endif