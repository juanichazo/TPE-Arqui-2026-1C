#include <soundDriver.h>
#include <time.h>

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);

#define PIT_BASE 1193182

static void sd_play_sound(uint32_t freq)
{
    if(freq == 0) return;
    uint16_t div = (uint16_t)(PIT_BASE / freq);

    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(div & 0xff));
    outb(0x42, (uint8_t)(div >> 8));

    uint8_t tmp = inb(0x61);
    outb(0x61, tmp | 0x3);
}

static void sd_nosound(void)
{
    uint8_t tmp = inb(0x61);
    outb(0x61, tmp & 0xfc);
}

void sd_beep(uint32_t freq, uint64_t time)
{
    sd_play_sound(freq);
    sleep(time);
    sd_nosound();
}
