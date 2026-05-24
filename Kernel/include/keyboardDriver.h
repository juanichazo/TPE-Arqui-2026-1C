#include <stdint.h>

uint64_t keyboard_read(char *buffer, uint64_t maxLen);
void saveKeyToBuffer();
char keyboard_get_char();