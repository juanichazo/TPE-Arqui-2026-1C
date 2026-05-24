#include <stdint.h>
#include <time.h>

extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);

#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS   0x04

static uint8_t read_rtc_register(uint8_t reg) {
    outb(0x70, reg);       // Le digo al puerto 70h qué registro quiero
    return inb(0x71);      // Leo la respuesta del puerto 71h
}

static uint8_t bcdToDec(uint8_t bcd) {
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

uint8_t getSeconds() {
    return bcdToDec(read_rtc_register(RTC_SECONDS));
}

uint8_t getMinutes() {
    return bcdToDec(read_rtc_register(RTC_MINUTES));
}

uint8_t getHours() {
    return bcdToDec(read_rtc_register(RTC_HOURS));
}

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}
