#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>

void putChar(char c);
void print(char* string);
void puts(char* string);
char getChar();
int readLine(char * buffer, int maxSize);
void drawPixel(uint32_t color, uint64_t x, uint64_t y);
void getTime();
char getCharNoWait();
void sleep(uint64_t ticks);
void drawRectangle(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color);
void printRegisters();
int strncontains(char* string, uint64_t size, char* expression);
void strtok(char* string, uint64_t size, char delim, char* tokens[]);
uint64_t string_to_int(char* string);
char* int_to_str(uint64_t num, char* string);

#endif