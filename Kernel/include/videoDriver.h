#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void drawChar(char c, uint64_t x, uint64_t y, uint64_t text_color, uint64_t bg_color);

int getScreenWidth();
int getScreenHeight();

#endif