#include <videoDriver.h>
#include <lib.h>
#include <keyboardDriver.h>
#include <time.h>

int nextCharPos = 0;
// color de texto
uint32_t text_color = 0xFFFFFF;
//color del fondo
uint32_t bg_color = 0x000000;

#define SCREEN_WIDTH 100
#define SCREEN_LENGTH 45

typedef struct{
	char c;
	uint64_t text_col;
	uint64_t bg_col;
} Cell;

Cell buffer[SCREEN_LENGTH][SCREEN_WIDTH] = {0}; // TODO: Habría que adaptarlo al tamaño de la pantalla

int currentX = 0;
int currentY = 0;

void nextPos();
void prevPos();

void set_bg_color(uint32_t new_bg_color){
	bg_color = new_bg_color;
}

void set_text_color(uint32_t new_text_color){
	text_color = new_text_color;
}

void putChar(char c){
	if(c == '\n'){
		nextLine();
		return;
	}
	if(c == '\b'){
		drawChar(0, currentX, currentY, text_color, bg_color);
		prevPos();
		buffer[currentY % SCREEN_LENGTH][currentX].c = 0;
	} else 
		buffer[currentY % SCREEN_LENGTH][currentX].c = c;
		
	buffer[currentY % SCREEN_LENGTH][currentX].bg_col = bg_color;
	buffer[currentY % SCREEN_LENGTH][currentX].text_col = text_color;

	drawChar(c, currentX, currentY, text_color, bg_color);
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
	print(string, 0xFFFFFFFF);
	nextLine();
}

void nextPos(){
	currentX = (currentX+1) % getScreenWidth();
	if(currentX == 0){
		nextLine();
	} 
}

void prevPos(){
	if(currentX == 0){
		currentX = getScreenWidth();
		currentY--;
	} else {
		currentX--;
	}
}

void nextLine(){
	currentX = 0;
	if(currentY >= SCREEN_LENGTH - 1){
		for(int i = 0; i < currentY; i++){
			memcpy(buffer[i], buffer[i+1], SCREEN_WIDTH * sizeof(Cell));
		}
		memset(buffer[currentY], 0, SCREEN_WIDTH * sizeof(Cell));
		redraw();
	} else {
		currentY++;
	}
}

void toggle_cursor(){
	if(ticks_elapsed() % 20 < 10){
		drawChar('_', currentX, currentY, text_color, bg_color);
	} else {
		drawChar(' ', currentX, currentY, bg_color, bg_color);
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
	drawChar(' ', currentX, currentY, text_color, bg_color);
	return chars_read;
}

// version que también escribe en pantalla lo que escribo
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
	int max = currentY;
	currentX = 0;
	currentY = 0;
	uint64_t backup_bg_col = bg_color;
	uint64_t backup_text_col = text_color;

	for(int i = 0; i <= max; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			drawChar(buffer[i][j].c, currentX, currentY, buffer[i][j].text_col, buffer[i][j].bg_col);
			nextPos();
		}
		if(currentX != 0){
			currentY++;
			currentX = 0; 
		}
	}
	bg_color = backup_bg_col;
	text_color = backup_text_col;
	currentY = max;
	currentX = 0;
}


