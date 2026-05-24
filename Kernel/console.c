#include <videoDriver.h>
#include <lib.h>
#include <keyboardDriver.h>
#include <time.h>

// Archivo con los mapas para cada char de la fuente usada
//#include <ubuntuMono.h>

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
	buffer[currentY % SCREEN_LENGTH][currentX].c = c;
	buffer[currentY % SCREEN_LENGTH][currentX].bg_col = bg_color;
	buffer[currentY % SCREEN_LENGTH][currentX].text_col = text_color;

	drawChar(c, currentX, currentY, text_color, bg_color);
	nextPos();
}

void print(char* string){
	while(*string){
        if(*string == '\n'){
            nextLine();
        } else { 
		    putChar(*string);
        }
		string++;
	}
}

void puts(char* string){
	print(string);
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

char active = 0;
char prev_ticks = 0;

void toggle_cursor(){
	if(ticks_elapsed() % 20 < 10){
		putChar('_');
	} else {
		putChar(0);
	}
	prevPos();
}

void readLine(char* buffer, uint64_t max){
	uint64_t count = 0;
	char key = 0;

	while(count < max){
		toggle_cursor();

		if(keyboard_read(&key, 1) && key){
			switch (key){
				case '\b':
					if(count == 0) 
						break;
					putChar(0);
					prevPos(); prevPos();
					putChar(0);
					prevPos();
					*(--buffer) = 0;	
					break;
				
				case '\n':
					putChar(0);
					nextLine();
					*buffer = 0;
					return count;
				default:
					putChar(key);	
					*(buffer++) = key;
					count++;
					break;
			}
		}
	}
	*buffer = 0;
	return count;
}

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

int runConsole(){
	char key;

	while(1){
		if(keyboard_read(&key, 1) && key)
			putChar(key);
	}
}

