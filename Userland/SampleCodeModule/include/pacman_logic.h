#ifndef PACMAN_LOGIC_H
#define PACMAN_LOGIC_H

#include "pacman_types.h"

void initGame(int two_players_mode);
void moveEntity(Entity *e);
void moveGhostAI(Entity *ghost, int ghost_id);
void handleCollision(void);
int  dirPassable(int x, int y, Direction d);

#endif
