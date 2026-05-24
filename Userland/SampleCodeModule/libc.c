#include "include/user_syscalls.h"
#include "include/libc.h"

void putChar(char c) {
    sys_write(STDOUT, &c, 1);
}

void print(char* string) {
    while (*string != 0) {
        putChar(*string);
        string++;
    }
}

char getChar() {
    char c = 0;
    sys_read(STDIN, &c, 1);
    return c;
}

int readLine(char * buffer, int maxSize) {
    int index = 0;
    char c;

    while (index < maxSize - 1) {
        c = getChar(); 

        if (c == '\n') {
            putChar('\n');
            break;
            
        } else if (c == '\b') {
            if (index > 0) {
                index--;
                putChar('\b');
                putChar(' ');
                putChar('\b');
            }
            
        } else if (c != 0) { 
            buffer[index] = c;
            index++;
            putChar(c);
        }
    }

    buffer[index] = 0; 
    
    return index;
}

void drawPixel(uint32_t color, uint64_t x, uint64_t y) {
    sys_draw(color, x, y);
}