#ifndef PACMAN_TYPES_H
#define PACMAN_TYPES_H

#include <stdint.h>

#define NUM_GHOSTS 4

#define COLOR_PACMAN 0xFFFF00
#define COLOR_BLINKY 0xFF0000
#define COLOR_PINKY  0xFFB8FF
#define COLOR_INKY   0x00FFFF
#define COLOR_CLYDE  0xFFB852

#define Y_RESOLUTION 768
#define X_RESOLUTION 1024

#define MAP_WIDTH          28
#define MAP_HEIGHT         31
#define BLOCK_SIZE         12
#define SCALE              2
#define OFFSET_X           176
#define OFFSET_Y           12
#define TILEMAP_ROW_OFFSET 3

#define SCORE_SMALL_DOT  10
#define SCORE_BIG_DOT    50
#define SCORE_GHOST_EAT  200
#define STARTING_LIVES   3

#define VIS_SPEED                4
#define FRIGHTEN_DURATION        80
#define COLOR_FRIGHTENED         0x0000CC
#define FRIGHTEN_FLASH_THRESHOLD 20

typedef enum { UP, DOWN, LEFT, RIGHT, NONE } Direction;

typedef enum { MENU, PLAYING_1P, PLAYING_2P, GAME_OVER, WIN } GameState;

typedef struct {
    int x, y;
    int prev_x, prev_y;
    Direction current_dir;
    uint32_t  color;
    int is_player_2;
    int in_house;
    int frightened;
    int frighten_timer;
    int returning;
    int vis_x, vis_y;
    int prev_vis_x, prev_vis_y;
    Direction next_dir;
} Entity;

/* Variables globales definidas en pacman.c */
extern int       map[MAP_HEIGHT][MAP_WIDTH];
extern Entity    pacman;
extern Entity    ghosts[NUM_GHOSTS];
extern int       score;
extern int       lives;
extern int       total_dots;
extern int       dots_eaten;
extern int       game_tick;
extern GameState currentState;
extern int       pacman_mouth_frame;

#endif
