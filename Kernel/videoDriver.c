#include <videoDriver.h>
#include <lib.h>

uint32_t* memset32(uint32_t*, uint32_t, uint64_t);

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}

void drawFromArray(uint64_t* bmp, uint64_t x, uint64_t y, uint64_t width, uint64_t height) {
	for(int j = y; j < y + width; j++){
        for(int i = x; i < x + width; i++){
			putPixel(*(bmp++), i, j);
        }
    }
    
}

#include <ubuntuMono.h>

void drawRect(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint64_t color){
	uint64_t bytesPerPixel = VBE_mode_info->bpp / 8;

	if (x2 >= VBE_mode_info->width)
		x2 = VBE_mode_info->width - 1;
	if (y2 >= VBE_mode_info->height)
		y2 = VBE_mode_info->height - 1;

	uint64_t width = (x2 - x1 + 1) * 3;
	uint8_t blue = color & 0xFF;
	uint8_t green = (color >> 8) & 0xFF;
	uint8_t red = (color >> 16) & 0xFF;

    uint8_t* row = ((uint8_t *) VBE_mode_info->framebuffer) + y1 * VBE_mode_info->pitch + x1 * bytesPerPixel;

	for (uint64_t y = y1; y <= y2; y++) {
		for(uint64_t x = 0; x < width; x += bytesPerPixel){
			row[x] = blue;
			row[x+1] = green;
			row[x+2] = red;
		}
		row += VBE_mode_info->pitch;
	}
}

void scrollScreenUp(uint64_t pixels){
	uint8_t * bufferWrite = (uint8_t *) VBE_mode_info->framebuffer;
	uint8_t * bufferRead = (uint8_t *) VBE_mode_info->framebuffer + (VBE_mode_info->pitch) * pixels;
	for(int y1 = 0, y2 = pixels; y2 < VBE_mode_info->height; y1++, y2++){
		memcpy(bufferWrite, bufferRead, VBE_mode_info->pitch);
		bufferRead += VBE_mode_info->pitch;
		bufferWrite += VBE_mode_info->pitch;
	}
}

void drawChar(char c, uint64_t x, uint64_t y, uint64_t text_color, uint64_t bg_color, uint64_t char_size){
	int offset_x = x * ubuntuMono_inf.width * char_size + 1;
	int offset_y = y * ubuntuMono_inf.height * char_size + 1;
	int ch = (int)c;

	for(int j = 0; j < ubuntuMono_inf.height * char_size; j++){
		for(int i = 0; i < ubuntuMono_inf.width * char_size; i++){
			if(ubuntuMono_bmp[ch][j / char_size] & (1 << (i/char_size)))
				putPixel(text_color, i + offset_x, j + offset_y);
			else
				putPixel(bg_color, i + offset_x, j + offset_y);
		}
	}
}

int getScreenWidth(){
	return VBE_mode_info->width;
}

int getScreenHeight(){
	return VBE_mode_info->height;
}
