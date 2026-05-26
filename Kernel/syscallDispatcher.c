#include <stdint.h>
#include <console.h>
#include <time.h>
#include <keyboardDriver.h>
#include <videoDriver.h>

extern void _hlt();

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STDINRAW 3

uint64_t sys_read(uint64_t fd, char * buffer, uint64_t count) {
    if (fd == STDIN) {
        readLine(buffer, count);        
    }    
    return -1;
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
    return -1;
}

uint64_t sys_time(uint64_t selector) {
    switch (selector) {
        case 0: return getHours();
        case 1: return getMinutes();
        case 2: return getSeconds();
        default: return -1;
    }
}

uint64_t sys_draw(uint32_t color, uint64_t x, uint64_t y){
    if (x < getScreenWidth() && y < getScreenHeight()) {
        putPixel(color, x, y);
        return 0; 
    }
    
    return -1;
}

uint64_t hashcode = 0x1234;

uint64_t sys_sethash(uint64_t new_hash){
    return hashcode = new_hash;
}

uint64_t sys_gethash(unsigned char *str) {
    unsigned long hash = hashcode;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

uint64_t sys_setcolor(uint64_t text, uint64_t background){
    set_text_color(text & 0xFFFFFF);
    set_bg_color(background & 0xFFFFFF);
    return 1;
}

uint64_t sys_setsize(uint64_t text_size){
    float size = (float)text_size / 10;
    if(size < 0.7 || size > 3.5) 
        return 1;
    set_text_size(size);
    return 0;
}

uint64_t (*syscalls[8])(uint64_t, uint64_t, uint64_t) = {
    sys_read, sys_write, sys_time, sys_draw, sys_sethash, sys_gethash, sys_setcolor, sys_setsize
};

uint64_t syscallDispatcher(uint64_t syscall, uint64_t p1, uint64_t p2, uint64_t p3) {
    
    if(syscall < sizeof(syscalls) / sizeof(syscalls[0])){
        return syscalls[syscall](p1, p2, p3);
    } 
    
    puts("[syscall unknown]"); // TODO podríamos hacer que esto sea una excepción
    return -1;
}