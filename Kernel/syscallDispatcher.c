#include <stdint.h>
#include <naiveConsole.h>

#define STDOUT 1
#define STDERR 2

void sys_write(uint64_t fd, char * buffer, uint64_t count) {
    if (fd == STDOUT) {
        for (int i = 0; i < count; i++) {
            putChar(buffer[i]);
        }
    } else if (fd == STDERR) {
        for (int i = 0; i < count; i++) {
            putChar(buffer[i]);
        }
    }
}

void syscallDispatcher(uint64_t syscall, uint64_t p1, uint64_t p2, uint64_t p3) {
    switch (syscall) {
        case 1:
            sys_write(p1, (char *)p2, p3);
            break;
    }
}