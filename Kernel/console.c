#include <videoDriver.h>

int nextCharPos = 0;
uint32_t fc = 0xFFFFFF;
uint32_t bc = 0x000000;

// Archivo con los mapas para cada char de la fuente usada
#include <ubuntuMono.h>

void putChar(char c, uint64_t offset_x, uint64_t offset_y, uint32_t fg, uint32_t bg){

	for(int j = 0; j < ubuntuMono_inf.height; j++){
		for(int i = 0; i < ubuntuMono_inf.width; i++){
			if(ubuntuMono_bmp[c][j] & (1 << i)){
				putPixel(fg, i + offset_x, j + offset_y);
			} else {
				putPixel(bg, i + offset_x, j + offset_y);
			}
		}
	}
}

void puts(char* string){
    int width = getScreenWidth();
    int x = (nextCharPos % width);
    int y = (nextCharPos / width) * 17;
	do{
        if(*string == '\n'){
            nextCharPos = (nextCharPos + width);
            nextCharPos -= nextCharPos % width;
        } else { 
		    putChar(*string, x + 1, y + 1, fc, bc);
            nextCharPos+=8;
        }
		x = (nextCharPos % width);
        y = (nextCharPos / width) * 17;
	}while(*(string++));
    nextCharPos = (nextCharPos + width);
    nextCharPos -= nextCharPos % width;
}


