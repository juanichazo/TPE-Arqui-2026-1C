#include <videoDriver.h>
#include <keyboardDriver.h>
#include <time.h>

// Archivo con los mapas para cada char de la fuente usada
#include <ubuntuMono.h>

int nextCharPos = 0;
// color de texto
uint32_t text_color = 0xFFFFFF;
//color del fondo
uint32_t bg_color = 0x000000;

//char buffer[1000][1000]; // TODO: Habría que adaptarlo al tamaño de la pantalla

int currentX = 0;
int currentY = 0;

void set_bg_color(uint32_t new_bg_color){
	bg_color = new_bg_color;
}

void set_text_color(uint32_t new_text_color){
	text_color = new_text_color;
}

void putChar(char c){

	int offset_x = currentX * 8 + 1;
	int offset_y = currentY * 17 + 1;

	for(int j = 0; j < ubuntuMono_inf.height; j++){
		for(int i = 0; i < ubuntuMono_inf.width; i++){
			if(ubuntuMono_bmp[c][j] & (1 << i)){
				putPixel(text_color, i + offset_x, j + offset_y);
			} else {
				putPixel(bg_color, i + offset_x, j + offset_y);
			}
		}
	}
	nextPos();
}

void print(char* string){
	do{
        if(*string == '\n'){
            currentY++;
			currentX = 0;
        } else { 
		    putChar(*string);
        }
	}while(*(++string));
}

void puts(char* string){
	print(string);
	currentY++;
	currentX = 0;
}

void nextPos(){
	currentX = (currentX+1) % getScreenWidth();
	if(currentX == 0) currentY++;;
}

void prevPos(){
	if(currentX == 0){
		currentX = getScreenWidth();
		currentY--;
	} else {
		currentX--;
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
					currentY++;
					currentX = 0;
					return count;
				default:
					putChar(key);	
					*(buffer++) = key;
					count++;
					break;
			}
		}
	}

	return count;
}


int runConsole(){
	char key;

	while(1){
		if(keyboard_read(&key, 1) && key)
			putChar(key);
	}
}

