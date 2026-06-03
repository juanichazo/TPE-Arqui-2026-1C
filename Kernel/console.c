#include <videoDriver.h>
#include <lib.h>
#include <keyboardDriver.h>
#include <time.h>
#include <console.h>

// color de texto
uint32_t text_color = 0xFFFFFF;
//color del fondo
uint32_t bg_color = 0x000000;

#define BUFFER_WIDTH 40000

char buffer[BUFFER_WIDTH] = {0}; 

int char_size = 1;
int currentX = 0;
int currentY = 0;

void nextPos();
void prevPos();
void nextLine();

void set_bg_color(uint32_t new_bg_color){
	bg_color = new_bg_color;
}

void set_text_color(uint32_t new_text_color){
	text_color = new_text_color;
}

void set_text_size(uint64_t size){
	if(size == char_size) return;
	char_size = size;
	currentY = 0;
	drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
}

void putChar(char c){
	if(c == '\n'){
		nextLine();
		return;
	}
	if(c == 14){ // 14 es el char correspondiente a borrar la pantalla
		clearScreen();
		return;
	}

	if(c == '\b'){
		drawChar(0, currentX, currentY, text_color, bg_color, char_size);
		prevPos();
	} 
		

	drawChar(c, currentX, currentY, text_color, bg_color, char_size);
	if(c != '\b')
		nextPos();

}

uint64_t print(char* string, uint64_t count){
	uint64_t i = 0;
	while(*string && (i++) <= count){
		putChar(*(string++));
	}
	return i;
}

void puts(char* string){
	print(string, 0xFFFFFF);
	nextLine();
}

void nextPos(){
	currentX = (currentX+1) % (int)(getScreenWidth() / (char_size * 8));
	if(currentX == 0){
		nextLine();
	} 
}

void prevPos(){
	if(currentX == 0){
		currentX = getScreenWidth() / (char_size * 8) - 1;
		currentY--;
	} else {
		currentX--;
	}
}

void nextLine(){
	currentX = 0;
	if(currentY >= (getScreenHeight() / (17 * char_size)) - 5){
		redraw();
	} else {
		currentY++;
	}
}

void toggle_cursor(){
	if(ticks_elapsed() % 20 < 10){
		drawChar('_', currentX, currentY, text_color, bg_color, char_size);
	} else {
		drawChar(' ', currentX, currentY, bg_color, bg_color, char_size);
	}
}

uint64_t readLine(char* buffer, uint64_t max){
	uint64_t chars_read = 0;
    char c;    
	while (chars_read < max) {
		toggle_cursor();
		if ((c = keyboard_get_char())) {
			buffer[chars_read++] = c;
		} else {
			// Si el buffer está vacío duermo la CPU hasta que el teclado dispare una nueva interrupción 
			_hlt(); 
		}
	}
	drawChar(' ', currentX, currentY, text_color, bg_color, char_size);
	return chars_read;
}

void redraw(){
	scrollScreenUp(17 * char_size);
}

void clearScreen(){
	drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
	currentY=0;
	currentX=0;
}



