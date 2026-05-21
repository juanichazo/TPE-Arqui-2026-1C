#include <stdint.h>
#include <naiveConsole.h>

    for (uint64_t i = 0; i < count; i++) {
        ncPrintChar(buffer[i]);
    }

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
        case SYS_WRITE:
            return sys_write(p1, (char *)p2, p3);
        case SYS_READ:
            return sys_read(p1, (char *)p2, p3);
        case SYS_EXIT:
            return sys_exit(p1);
        default:
            ncPrint("[syscall unknown]\n");
            break;
    }
}