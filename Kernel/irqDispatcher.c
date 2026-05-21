#include <time.h>
#include <stdint.h>
#include <lib.h>
#include <keyboard.h>
#include <console.h>

static void int_20();
static void int_21();

#define KEYBOARD_BUFFER_SIZE 128

static char keyboardBuffer[KEYBOARD_BUFFER_SIZE];
static uint64_t keyboardReadIndex = 0;
static uint64_t keyboardWriteIndex = 0;

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
        return; // buffer full, drop char
    }
    keyboardBuffer[keyboardWriteIndex] = c;
    keyboardWriteIndex = next;
}

uint64_t keyboard_read(char *buffer, uint64_t maxLen) {
    uint64_t count = 0;

    while (count < maxLen) {
        while (keyboard_buffer_available() == 0) {
            // Busy-wait until a key arrives. IRQ1 will keep filling the buffer.
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

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 0:
			int_20();
			break;
		case 1:
			int_21();
			break;
	}
	return;
}

void int_20() {
	timer_handler();
}

void int_21(){
	uint8_t scancode = inb(0x60);

    if (scancode < 0x80) {
        char c = scancodeToAscii[scancode];
        if (c != 0) {
            keyboard_push(c);
            ncPrintChar(c);
        }
    }
}
