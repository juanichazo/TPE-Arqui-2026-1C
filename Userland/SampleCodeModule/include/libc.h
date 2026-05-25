#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>

void putChar(char c);
void print(char* string);
void puts(char* string);
char getChar();
int readLine(char * buffer, int maxSize);
void drawPixel(uint32_t color, uint64_t x, uint64_t y);

#endif