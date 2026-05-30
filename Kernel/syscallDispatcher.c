#include <stdint.h>
#include <console.h>
#include <time.h>
#include <keyboardDriver.h>
#include <videoDriver.h>
#include <lib.h>
#include <interrupts.h>
#include <soundDriver.h>

extern void _hlt();

enum SYSCALL_RESULTS {ENOSYS = -1, SUCCESS, FAILURE};
enum STREAMS {STDIN, STDOUT, STDERR, STDINRAW};

typedef uint64_t (*SyscallHandler)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t sys_read(uint64_t fd, char * buffer, uint64_t count) {
    if (fd == STDIN) {
        readLine(buffer, count);        
    }    
    if (fd == STDINRAW) { 
        char c = keyboard_get_char(); 
        buffer[0] = c;
        return c != 0;
    }
    return FAILURE;
}

uint64_t sys_write(uint64_t fd, char * buffer, uint64_t count) {
    // Verificamos que sea STDOUT (1) o STDERR (2)
    if (fd == 1) {   
        return print(buffer, count);
    }
    if(fd == 2){
        set_text_color(0xFF3030);
        uint64_t ret = print(buffer, count);
        set_text_color(0xFFFFFF);
        return ret;
    }
    return FAILURE;
}

uint64_t sys_time(uint64_t selector) {
    switch (selector) {
        case 0: return getHours();
        case 1: return getMinutes();
        case 2: return getSeconds();
        default: return FAILURE;
    }
}

uint64_t sys_draw(uint32_t color, uint64_t x, uint64_t y){
    if (x < getScreenWidth() && y < getScreenHeight()) {
        putPixel(color, x, y);
        return SUCCESS; 
    }
    return FAILURE;
}

uint64_t hashcode = 0x1234;

uint64_t sys_sethash(uint64_t new_hash){
    return hashcode = new_hash;
}

uint64_t sys_gethash() {
    return hashcode;
}

uint64_t sys_setcolor(uint64_t text, uint64_t background){
    set_text_color(text & 0xFFFFFF);
    set_bg_color(background & 0xFFFFFF);
    return SUCCESS;
}

uint64_t sys_setsize(uint64_t text_size){
    if(text_size < 1 || text_size > 3) 
        return FAILURE;
    set_text_size(text_size);
    return SUCCESS;
}

uint64_t sys_sleep(uint64_t ticks_to_wait, uint64_t p2, uint64_t p3) {
    sleep(ticks_to_wait);
    return SUCCESS;
}

uint64_t sys_draw_rect(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint64_t color) {
    drawRect(x1, y1, x2, y2, color);
    return SUCCESS;
}

uint64_t sys_regs(){
    printRegisters();
    return SUCCESS;
}

uint64_t sys_draw_buffer(uint64_t* bmp, uint64_t x, uint64_t y, uint64_t width, uint64_t height){
    drawFromArray(bmp, x, y, width, height);    
    return SUCCESS; 
}

uint64_t sys_play_sound(uint64_t freq, uint64_t time_in_ticks){
    sd_beep((uint32_t)freq, time_in_ticks);
    return SUCCESS;
}

SyscallHandler syscalls[] = {
    (SyscallHandler)sys_read,        
    (SyscallHandler)sys_write,       
    (SyscallHandler)sys_time,        
    (SyscallHandler)sys_draw,        
    (SyscallHandler)sys_sethash,     
    (SyscallHandler)sys_gethash,     
    (SyscallHandler)sys_setcolor,    
    (SyscallHandler)sys_setsize,     
    (SyscallHandler)sys_sleep,       
    (SyscallHandler)sys_draw_rect,
    (SyscallHandler)sys_regs,
    (SyscallHandler)sys_draw_buffer,
    (SyscallHandler)sys_play_sound
};

uint64_t syscallDispatcher(uint64_t syscall, uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5) {
    if(syscall < sizeof(syscalls) / sizeof(syscalls[0]))
        return syscalls[syscall](p1, p2, p3, p4, p5);
        
    puts("[syscall unknown]");
    return ENOSYS;
}