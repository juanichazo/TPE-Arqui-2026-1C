#ifndef KERNEL_CRACKER_H
#define KERNEL_CRACKER_H

#include <stdint.h>
#include <user_syscalls.h>

uint64_t reset_tsc();
uint64_t hash_asm(char* str);

uint64_t hash(char *str);
void printHash(char* params[]);
uint64_t bruteforce_hashcode();
void sethash(char* params[]);
void get_next(char* str, uint64_t max);

#endif