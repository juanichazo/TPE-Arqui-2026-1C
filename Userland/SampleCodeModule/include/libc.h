#ifndef _LIBC_H_
#define _LIBC_H_

#include <stdint.h>

void putChar(char c);
void print(char* string);
char getChar();
int readLine(char * buffer, int maxSize);

#endif