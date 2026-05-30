#ifndef KEYBOARD_DRIVIER_H_
#define KEYBOARD_DRIVIER_H_

#include <stdint.h>

uint64_t keyboard_read(char *buffer, uint64_t maxLen);
void saveKeyToBuffer();
char keyboard_get_char();

#endif