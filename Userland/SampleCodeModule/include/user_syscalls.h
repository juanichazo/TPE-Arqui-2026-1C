#ifndef _USER_SYSCALLS_H_
#define _USER_SYSCALLS_H_

#include <stdint.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

uint64_t sys_read(uint64_t fd, char * buffer, uint64_t count);
uint64_t sys_write(uint64_t fd, char* buffer, uint64_t count);
uint64_t sys_time(uint64_t selector);
uint64_t sys_draw(uint32_t color, uint64_t x, uint64_t y);
uint64_t sys_sethash(uint64_t new_hash);
uint64_t sys_gethash();
uint64_t sys_setcolor(uint64_t text, uint64_t bg);
uint64_t sys_settextsize(uint64_t text_size);
uint64_t sys_draw_buffer(uint64_t* bmp, uint64_t x, uint64_t y, uint64_t width, uint64_t height);
uint64_t sys_play_sound(uint64_t freq, uint64_t time);
uint64_t sys_play_music(uint32_t* freqs, uint64_t* durations, uint64_t count);
uint64_t reset_tsc();

#endif