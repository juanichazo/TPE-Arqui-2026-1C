#include <stdint.h>
#include <lib.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6
 
extern void puts(char* string);

void exceptionDispatcher(int exception, uint64_t *registers) {
    if (exception == ZERO_EXCEPTION_ID){
        puts("\nEXCEPCION: DIVISION POR CERO\n");
    }
    else if (exception == INVALID_OPCODE_EXCEPTION_ID) {
        puts("\nEXCEPCION: CODIGO DE OPERACION INVALIDO\n");
    } else {
        puts("EXCEPCION DESCONOCIDA");
    }
    
    printRegisters(registers);
    puts("\nRecuperando sistema... Volviendo a la Shell.\n");
}