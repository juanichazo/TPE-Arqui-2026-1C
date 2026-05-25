#include <stdint.h>
#include <naiveConsole.h>
#include <time.h>
#include <keyboardDriver.h>
#include <videoDriver.h>

extern void _hlt();

#define STDIN 0
#define STDOUT 1
#define STDERR 2

extern int getScreenWidth();
extern int getScreenHeight();
extern void drawChar(char c, uint64_t x, uint64_t y, uint64_t text_color, uint64_t bg_color);

uint64_t sys_read(uint64_t fd, char * buffer, uint64_t count) {
    if (fd == STDIN) {
        uint64_t chars_read = 0;
        
        while (chars_read < count) {
            char c = keyboard_get_char();
            
            if (c != 0) {
                buffer[chars_read] = c;
                chars_read++;
            } else {
                // Si el buffer está vacío duermo la CPU hasta que el teclado dispare una nueva interrupción 
                _hlt(); 
            }
        }
        return chars_read;
    }
    
    return -1;
}

static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;

uint64_t sys_write(uint64_t fd, char * buffer, uint64_t count) {

    // Verificamos que sea STDOUT (1) o STDERR (2)
    if (fd == 1 || fd == 2) { 
        
        for (uint64_t i = 0; i < count; i++) {
            char c = buffer[i];
            
            if (c == '\n') {
                cursor_x = 0;
                cursor_y++;
            } else if (c == '\b') {
                if (cursor_x > 0) {
                    cursor_x--;
                    drawChar(' ', cursor_x, cursor_y, 0x000000, 0x000000); 
                }
            } else {
                drawChar(c, cursor_x, cursor_y, 0xFFFFFF, 0x000000);
                cursor_x++;
            }

            if (cursor_x >= (getScreenWidth() / 8)) {
                cursor_x = 0;
                cursor_y++;
            }
        }
        
        return count;
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

uint64_t syscallDispatcher(uint64_t syscall, uint64_t p1, uint64_t p2, uint64_t p3) {
    
    switch (syscall) {
        case 0:
            return sys_read(p1, (char *)p2, p3);
        case 1:
            return sys_write(p1, (char *)p2, p3);
        case 2:
            return sys_time(p1);
        case 3:
            return sys_draw(p1, p2, p3);
        default:
            ncPrint("[syscall unknown]\n");
            return -1;
    }
}