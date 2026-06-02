#include <stdint.h>
#include <lib.h>
#include <interrupts.h>
#include <console.h>

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

static const char *regNames[] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", 
    "R11", "R12", "R13", "R14", "R15", "RIP", "CS ", "RFLAGS", "RSP", "SS"
};

void uint64ToHexStr(uint64_t num, char *str) {
    str[0] = '0';
    str[1] = 'x';
    for (int i = 7; i >= 0; i--) {
        int digit = num & 0xF;
        str[i + 2] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
        num >>= 4;
    }
    str[18] = '\0'; 
}

static uint64_t regs_for_printing[20] = {0};
uint8_t empty = 1;

void savedRegs(){
	empty = 0;
	uint64_t* saved = getregs();
	for(int i = 0; i < 20; i++){
		regs_for_printing[i] = saved[i];
	}
}

void printRegisters() {
	if(empty){
		print("Registros no guardados, tocar tab para tomar una snapshot\n", 58);
		return;
	}
    char hexBuffer[25];
    for (int i = 0; i < 20; i++) {
        print((char*)regNames[i], 10);
        print(": ", 2);
        uint64ToHexStr(regs_for_printing[i], hexBuffer); 
        print(hexBuffer, 100);
        print("\n", 1);
    }
}