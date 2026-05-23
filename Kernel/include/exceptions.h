#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_
#include <stdint.h>

void exceptionDispatcher(int exception, uint64_t *registers);
extern void _exception00Handler();
extern void _exception06Handler();

#endif