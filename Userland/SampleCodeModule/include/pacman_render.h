#ifndef PACMAN_RENDER_H
#define PACMAN_RENDER_H

#include "pacman_types.h"

void drawRect(int startX, int startY, int width, int height, uint32_t color);
void drawMap(void);
void drawEntity(Entity *e);
void drawHUD(void);
void drawHUDIfChanged(void);
void renderFrame(void);
void advanceVis(Entity *e);
int  isVisAligned(Entity *e);

#endif
