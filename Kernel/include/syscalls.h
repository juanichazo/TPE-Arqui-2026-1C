#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

#define SYS_READ  0
#define SYS_WRITE 1
#define SYS_EXIT  2

#define STDIN  0
#define STDOUT 1
#define STDERR 2

uint64_t syscallDispatcher(uint64_t syscall, uint64_t p1, uint64_t p2, uint64_t p3, uint64_t p4, uint64_t p5);

#endif

