#ifndef _USER_SYSCALLS_H_
#define _USER_SYSCALLS_H_

#include <stdint.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

uint64_t sys_write(uint64_t fd, char* buffer, uint64_t count);
uint64_t sys_time(uint64_t selector);

#endif