#include <stdint.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

extern void print(char* string); 
extern void puts(char* string);

static void zero_division(uint64_t *registers);
static void invalid_opcode(uint64_t *registers);
static void uint64ToHexStr(uint64_t num, char *str);
extern void runConsole(void);

static const char *regNames[] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", 
    "R11", "R12", "R13", "R14", "R15", "RIP", "CS ", "RFLAGS", "RSP"
};

void exceptionDispatcher(int exception, uint64_t *registers) {
	if (exception == ZERO_EXCEPTION_ID){
		zero_division(registers);
	}
	else if (exception == INVALID_OPCODE_EXCEPTION_ID) {
        invalid_opcode(registers);
    }
}

static void zero_division(uint64_t *registers) {
    puts("\nEXCEPCION: DIVISION POR CERO\n\n");
    puts("Estado de los registros:\n");
    
    char hexBuffer[25]; // buffer para convertir numeros a texto

    // recorremos los 19 registros y los imprimimos
    for (int i = 0; i < 19; i++) {
        print((char*)regNames[i]);
        print(": ");
        uint64ToHexStr(registers[i], hexBuffer); 
        print(hexBuffer);
        print("\n");
    }

    puts("\nRecuperando sistema... Volviendo a la Shell.\n");
    for (volatile uint64_t i = 0; i < 1000000000; i++); // TODO: ver si cambiar esto por polling
}

static void invalid_opcode(uint64_t *registers) {
    puts("\nEXCEPCION: CODIGO DE OPERACION INVALIDO\n\n");
    puts("Estado de los registros:\n");
    
    char hexBuffer[25];

    for (int i = 0; i < 19; i++) {
        print((char*)regNames[i]);
        print(": ");
        uint64ToHexStr(registers[i], hexBuffer); 
        print(hexBuffer);
        print("\n");
    }

    puts("\nRecuperando sistema... Volviendo a la Shell.\n");
    for (volatile uint64_t i = 0; i < 1000000000; i++); // espero que el usuario lea
}

// Función auxiliar que convierte un uint64_t en string Hexadecimal
static void uint64ToHexStr(uint64_t num, char *str) {
    str[0] = '0';
    str[1] = 'x';
    for (int i = 15; i >= 0; i--) {
        int digit = num & 0xF;
        str[i + 2] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
        num >>= 4;
    }
    str[18] = '\0'; 
}