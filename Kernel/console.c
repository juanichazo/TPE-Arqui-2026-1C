#include <videoDriver.h>
#include <keyboardDriver.h>

// Archivo con los mapas para cada char de la fuente usada
#include <ubuntuMono.h>

int nextCharPos = 0;
// color de texto
uint32_t text_color = 0xFFFFFF;
//color del fondo
uint32_t bg_color = 0x000000;

char buffer[1000][1000]; // Habría que adaptarlo al tamaño de la pantalla

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
	int offset_y =  1;

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

void puts(char* string){
	do{
        if(*string == '\n'){
            currentY++;
			currentX = 0;
        } else { 
		    putChar(*string);
        }
	}while(*(string++));
	currentY++;
	currentX = 0;
}

void nextPos(){
	currentX = (currentX+1) % getScreenWidth();
	if(currentX == 0) currentY++;;
}


int runConsole(){
	char key;

	while(1){
		if(keyboard_read(&key, 1) && key)
			putChar(key);
	}
}

