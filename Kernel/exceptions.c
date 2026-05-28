#include <stdint.h>
#include <lib.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6
 
extern void puts(char* string);

void exceptionDispatcher(int exception, uint64_t *registers) {
    if (exception == ZERO_EXCEPTION_ID){
        puts("\nEXCEPCION: DIVISION POR CERO\n");
        printRegisters(registers);
    }
    else if (exception == INVALID_OPCODE_EXCEPTION_ID) {
        puts("\nEXCEPCION: CODIGO DE OPERACION INVALIDO\n");
        printRegisters(registers);
    }

    puts("\nRecuperando sistema... Volviendo a la Shell.\n");
    for (volatile uint64_t i = 0; i < 1000000000; i++); // espero que el usuario lea
}