#include <stdint.h>
#include <naiveConsole.h>
#include <time.h>
#include <keyboardDriver.h>

extern void _hlt();

#define STDIN 0
#define STDOUT 1
#define STDERR 2

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

uint64_t sys_write(uint64_t fd, char * buffer, uint64_t count) {
    if (fd == STDOUT || fd == STDERR) {
            for (uint64_t i = 0; i < count; i++) {
                putChar(buffer[i]);
            }
            // Retorna la cantidad de caracteres impresos
            return count;
        }

    //Si se pasa un File Descriptor invalido retorna -1
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

uint64_t syscallDispatcher(uint64_t syscall, uint64_t p1, uint64_t p2, uint64_t p3) {
    switch (syscall) {
        case 0:
            return sys_read(p1, (char *)p2, p3);
        case 1:
            return sys_write(p1, (char *)p2, p3);
        case 2:
            return sys_time(p1);
        default:
            ncPrint("[syscall unknown]\n");
            return -1;
    }
}