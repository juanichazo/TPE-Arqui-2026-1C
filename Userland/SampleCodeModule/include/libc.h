#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>
#include <stdarg.h>

void putChar(char c);
uint64_t print(char* string);
uint64_t printf(char* format, ...);
void puts(char* string);
char getChar();
uint64_t scanf(char* format, ...);
int readLine(char * buffer, int maxSize);
void drawPixel(uint32_t color, uint64_t x, uint64_t y);
void getTime();
char getCharNoWait();
void sleep(uint64_t ticks);
void drawRectangle(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color);
void setTextColor(uint32_t text_color, uint32_t bg_color);
void setTextSize(uint64_t size);
void clearScreen();
void printRegisters();
int strncontains(char* string, uint64_t size, char* expression);
void strtok(char* string, uint64_t size, char delim, char* tokens[]);
uint64_t string_to_int(char* string);
char* int_to_str(uint64_t num, char* string);

#endif