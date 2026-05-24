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