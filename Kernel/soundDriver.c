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

uint32_t* current_freqs;
uint64_t* current_durations;
uint64_t time_remaining;
uint64_t freqs_remianing;
uint8_t playing = 0;

void sd_play_music(uint32_t* freqs, uint64_t* durations, uint64_t count){
    current_freqs = freqs;
    current_durations = durations;
    freqs_remianing = count;
    
    if(freqs_remianing--){
        playing = 1;
        sd_play_sound(*(current_freqs++));
        time_remaining = *(current_durations++);
    }
}

void sd_music_step(){
    if(!playing) return;

    if(time_remaining-- <= 0){
        if(freqs_remianing <= 0){
            sd_nosound();
            playing = 0;
            return;
        }

        freqs_remianing--;
        
        sd_play_sound(*(current_freqs++));
        time_remaining = *(current_durations++);
    }
}
