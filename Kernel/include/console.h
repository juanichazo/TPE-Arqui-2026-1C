#include <stdint.h>

void set_bg_color(uint32_t new_bg_color);
void set_text_color(uint32_t new_text_color);
void putChar(char c);
void print(char* string);
void puts(char* string);
int runConsole();
void redraw();