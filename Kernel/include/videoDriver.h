#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void putChar(char c, uint64_t offset_x, uint64_t offset_y, uint32_t fg, uint32_t bg);
void puts(char* string);

#endif