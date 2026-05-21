#include <stdint.h>

char buffer = 0;

uint8_t getKeyboardStatus(){
    return 0;
}

void getKey(){
    return buffer;
}

void saveKey(){
    buffer = readKeyboard();
}