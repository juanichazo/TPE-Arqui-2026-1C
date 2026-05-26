#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();

uint8_t getSeconds();
uint8_t getMinutes();
uint8_t getHours();

void sleep(uint64_t ticks_to_wait);

#endif
