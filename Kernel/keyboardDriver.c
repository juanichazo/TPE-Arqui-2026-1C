#include <stdint.h>
#include <soundDriver.h>
#include <console.h>
#include <lib.h>

#define KEYBOARD_BUFFER_SIZE 128

extern uint8_t readKeyboard();

static char keyboardBuffer[KEYBOARD_BUFFER_SIZE];
static uint64_t keyboardReadIndex = 0;
static uint64_t keyboardWriteIndex = 0;

char current = 0;
int shifted = 0;

static const char scancodeToAscii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,   '*', 0,  ' ',
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static uint64_t keyboard_next(uint64_t index) {
    return (index + 1) % KEYBOARD_BUFFER_SIZE;
}

uint64_t keyboard_buffer_available(void) {
    if (keyboardWriteIndex >= keyboardReadIndex) {
        return keyboardWriteIndex - keyboardReadIndex;
    }
    return KEYBOARD_BUFFER_SIZE - (keyboardReadIndex - keyboardWriteIndex);
}

void keyboard_push(char c) {
    uint64_t next = keyboard_next(keyboardWriteIndex);
    if (next == keyboardReadIndex) {
        return; 
    }
    keyboardBuffer[keyboardWriteIndex] = c;
    keyboardWriteIndex = next;
}


uint64_t keyboard_read(char *buffer, uint64_t maxLen) {
    uint64_t count = 0;

    while (count < maxLen) {
        if (keyboard_buffer_available() == 0) {
            return 0; 
        }

        char c = keyboardBuffer[keyboardReadIndex];
        keyboardReadIndex = keyboard_next(keyboardReadIndex);
        buffer[count++] = c;

        if (c == '\n') {
            break;
        }
    }

    return count;
}

void saveKeyToBuffer(){
	uint8_t scancode = readKeyboard();
    if (scancode < 0x80) {
        char c = scancodeToAscii[scancode];
        if (c == '\t') {
            savedRegs();
            return;
        }
        if (c != 0) {
            keyboard_push(c);
            current = c;
        } 
    }
    if(scancode == 0x2A || scancode == 0x36)
        shifted = 1;
    else if (scancode == 0xAA || scancode == 0xB6)
        shifted = 0;
    else if (scancode == 0x1D || scancode == 0x38)
        sd_beep(880, 4);
}



char keyboard_get_char() {
    char c = current;
    current = 0;
    if(shifted && c <= 'z' && c >= 'a'){
        c = c - 'a' + 'A';
    }
    return c;
}