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

void puts(char* string) {
    print(string);
    putChar('\n');
}

char getChar() {
    char c = 0;
    sys_read(STDIN, &c, 1);
    return c;
}

int readLine(char * buffer, int maxSize) {
    int index = 0;
    char c;

    while (index < maxSize - 1) {
        c = getChar(); 

        if (c == '\n') {
            putChar('\n');
            break;
            
        } else if (c == '\b') {
            if (index > 0) {
                index--;
                putChar('\b');
            }
            
        } else if (c != 0) { 
            buffer[index] = c;
            index++;
            putChar(c);
        }
    }

    buffer[index] = 0; 
    
    return index;
}

void drawPixel(uint32_t color, uint64_t x, uint64_t y) {
    sys_draw(color, x, y);
}

void drawRectangle(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color) {
    sys_draw_rect(x1, y1, x2, y2, (uint64_t)color);
}
void getTime() {
    uint8_t hour = (uint8_t)sys_time(0);
    uint8_t min  = (uint8_t)sys_time(1);
    uint8_t sec  = (uint8_t)sys_time(2); 

    hour = (hour + 24 - 3) % 24; // esta 3 horas adelantado

    //imprimo en formato HH:MM:SS
    putChar((hour / 10) + '0'); // sumo el ascii del cero para pasar a ascii
    putChar((hour % 10) + '0');
    putChar(':');

    putChar((min / 10) + '0');
    putChar((min % 10) + '0');
    putChar(':');

    putChar((sec / 10) + '0');
    putChar((sec % 10) + '0');

    putChar('\n');
}

char getCharNoWait() {
    char c = 0;
    sys_read(3, &c, 1); // le paso STDINRAW = 3
    return c;
}

void sleep(uint64_t ticks) {
    sys_sleep(ticks);
}

static const char *regNames[] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", 
    "R11", "R12", "R13", "R14", "R15", "RIP", "CS ", "RFLAGS", "RSP"
};

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

void printRegisters() { // TODO optimizar (se repite codigo de exceptions.c)
    uint64_t * registerArr;
    char hexBuffer;

    get_registers(registerArr);

    puts("\nEstado actual de los registros en Userland:\n");
    for (int i = 0; i < 19; i++) {
        print((char*)regNames[i]);
        print(": ");
        uint64ToHexStr(registerArr[i], hexBuffer); 
        print(hexBuffer);
        print("\n");
    }
}

int strncontains(char* string, uint64_t size, char* expression){
    while(size--){
        if(*expression == 0)
            return 1;
        if(*(string++) != *(expression++))
            return 0;
    }
    return 0; 
}

void strtok(char* string, uint64_t size, char delim, char* tokens[]){
    int current = 0;
    while(*string){
        if(*string == delim){
            if(*(string+1) != 0 && current < size) tokens[current++] = string+1;
            *string = 0;
        }
        string++;
    }
}

uint64_t string_to_int(char* string){
    uint64_t toRet = 0;
    while(*string >= '0' && *string <= '9'){
        toRet *= 10;
        toRet += *(string++) - '0';
    }
    return toRet;
}

char* int_to_str(uint64_t num, char* string){
    int i = 0;
    while(num > 0){
        string[i++] = (char)(num % 10) + '0';
        num /= 10;
    }
    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = string[j];
        string[j] = string[k];
        string[k] = temp;
    }

    return string;
}