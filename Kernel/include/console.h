#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>

void set_bg_color(uint32_t new_bg_color);
void set_text_color(uint32_t new_text_color);
void set_text_size(uint64_t size);

void putChar(char c);
void puts(char* string);
uint64_t print(char* string, uint64_t count);

uint64_t readLine(char* buffer, uint64_t max);

void redraw();
void clearScreen();

#endif