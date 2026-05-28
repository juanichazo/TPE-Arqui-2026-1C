#include <videoDriver.h>
#include <lib.h>
#include <keyboardDriver.h>
#include <time.h>
#include <console.h>

// color de texto
uint32_t text_color = 0xFFFFFF;
//color del fondo
uint32_t bg_color = 0x000000;

#define BUFFER_WIDTH 100
#define BUFFER_HEIGHT 45

typedef struct{
	char c;
	uint64_t text_col;
	uint64_t bg_col;
} Cell;

Cell buffer[BUFFER_HEIGHT][BUFFER_WIDTH] = {0}; // TODO: Habría que adaptarlo al tamaño de la pantalla

int char_size = 1;
int currentX = 0;
int currentY = 0;
int buffer_start = 0;

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
		buffer[currentY % BUFFER_HEIGHT][currentX].c = 0;
		nextLine();
		return;
	}
	if(c == 14){ // 14 es el char correspondiente a borrar la pantalla
		memset(buffer, 0, BUFFER_WIDTH);
		drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
		currentY=0;
		return;
	}

	if(c == '\b'){
		drawChar(0, currentX, currentY, text_color, bg_color, char_size);
		prevPos();
		buffer[currentY % BUFFER_HEIGHT][currentX].c = 0;
	} else 
		buffer[currentY % BUFFER_HEIGHT][currentX].c = c;
		
	buffer[currentY % BUFFER_HEIGHT][currentX].bg_col = bg_color;
	buffer[currentY % BUFFER_HEIGHT][currentX].text_col = text_color;

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
		currentX = BUFFER_WIDTH;
		currentY--;
	} else {
		currentX--;
	}
}

void nextLine(){
	currentX = 0;
	if(currentY >= (getScreenHeight() / (17 * char_size)) - 5){
		/*
		for(int i = 0; i < currentY; i++){
			memcpy(buffer[i], buffer[i+1], BUFFER_WIDTH * sizeof(Cell));
		}
		memset(buffer[currentY], 0, BUFFER_WIDTH * sizeof(Cell)); */
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
		if (c = keyboard_get_char()) {
			buffer[chars_read] = c;
			chars_read++;
		} else {
			// Si el buffer está vacío duermo la CPU hasta que el teclado dispare una nueva interrupción 
			_hlt(); 
		}
	}
	drawChar(' ', currentX, currentY, text_color, bg_color, char_size);
	return chars_read;
}

// version que también escribe en pantalla lo que escribo (echo)
	/*while(count < max){
		toggle_cursor();

		if(keyboard_read(&key, 1) && key){
			switch (key){
				case '\b':
					if(count == 0) 
						break;
					drawChar(0, currentX, currentY, text_color, bg_color);
					putChar('\b');
					buffer[--count] = 0;	
					break;
				
				case '\n':
					drawChar(0, currentX, currentY, text_color, bg_color);
					putChar('\n');
					buffer[count] = 0;
					return count;
				default:
					putChar(key);	
					buffer[count++] = key;
					break;*/


void redraw(){
	scrollScreenUp(17 * char_size);

	/* VERSION ANTERIOR QUE GUARDA LOS COMANDOS VIEJOS
	//set_text_size(char_size * 1.3);
	int max = currentY;
	int min = currentY - (getScreenHeight() / (char_size * 17)) + 2;
	min = min < 0 ? 0 : min;
	currentX = 0;
	currentY = 0;
	uint64_t backup_bg_col = bg_color;
	uint64_t backup_text_col = text_color;

	memset(buffer[max - min], 0, BUFFER_WIDTH);
	for(int i = min; i <= max; i++){
		for(int j = 0; j < BUFFER_WIDTH && buffer[i][j].c; j++){
				drawChar(buffer[i][j].c, currentX, currentY, buffer[i][j].text_col, buffer[i][j].bg_col, char_size);
				currentX++;
			}
			currentY++;
			currentX = 0; 
	} 
	bg_color = backup_bg_col;
	text_color = backup_text_col;
	currentY = max - min;
	
	currentX = 0; */
}



