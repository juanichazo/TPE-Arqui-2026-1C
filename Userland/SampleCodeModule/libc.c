#include <user_syscalls.h>
#include <libc.h>   

uint64_t printInt(uint64_t n);
uint64_t printHex(uint64_t n);
uint64_t print(char* string);

uint64_t readString(char* string);
uint64_t readHex();
uint64_t readInt();
    
void putChar(char c) {
    sys_write(STDOUT, &c, 1);
}

uint64_t print(char* string) {
    uint64_t count = 0;
    while (*string != 0) {
        putChar(*string);
        string++;
        count++;
    }
    return count;
}

void puts(char* string) {
    print(string);
    putChar('\n');
}

char getChar() {
    char c = 0;
    sys_read(STDIN, &c, 1);
    return c;
}

int readLine(char * buffer, int maxSize) {
    int index = 0;
    char c;

    while (index < maxSize - 1) {
        c = getChar(); 

        if (c == '\n') {
            putChar('\n');
            break;
            
        } else if (c == '\b') {
            if (index > 0) {
                index--;
                putChar('\b');
            }
            
        } else if (c != 0) { 
            buffer[index] = c;
            index++;
            putChar(c);
        }
    }

    buffer[index] = 0; 
    
    return index;
}

void drawPixel(uint32_t color, uint64_t x, uint64_t y) {
    sys_draw(color, x, y);
}

void drawRectangle(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color) {
    sys_draw_rect(x1, y1, x2, y2, (uint64_t)color);
}

void getTime() {
    uint8_t hour = (uint8_t)sys_time(0);
    uint8_t min  = (uint8_t)sys_time(1);
    uint8_t sec  = (uint8_t)sys_time(2); 

    hour = (hour + 24 - 3) % 24; // esta 3 horas adelantado

    //imprimo en formato HH:MM:SS
    putChar((hour / 10) + '0'); // sumo el ascii del cero para pasar a ascii
    putChar((hour % 10) + '0');
    putChar(':');

    putChar((min / 10) + '0');
    putChar((min % 10) + '0');
    putChar(':');

    putChar((sec / 10) + '0');
    putChar((sec % 10) + '0');

    putChar('\n');
}

char getCharNoWait() {
    char c = 0;
    sys_read(3, &c, 1); // le paso STDINRAW = 3
    return c;
}

void sleep(uint64_t ticks) {
    sys_sleep(ticks);
}

void setTextColor(uint32_t text_color, uint32_t bg_color) {
    sys_setcolor(text_color, bg_color);
}

void setTextSize(uint64_t size) {
    sys_settextsize(size);
}

void clearScreen() {
    putChar(14); // Carácter especial para limpiar la consola de texto
}

void printRegisters(){
    sys_regs();
}

int strncontains(char* string, uint64_t size, char* expression){
    while(size--){
        if(*expression == 0)
            return 1;
        if(*(string++) != *(expression++))
            return 0;
    }
    return 0; 
}

void strtok(char* string, uint64_t size, char delim, char* tokens[]){
    int current = 0;
    while(*string){
        if(*string == delim){
            if(*(string+1) != 0 && current < size) tokens[current++] = string+1;
            *string = 0;
        }
        string++;
    }
}

uint64_t string_to_int(char* string){
    uint64_t toRet = 0;
    while(*string >= '0' && *string <= '9'){
        toRet *= 10;
        toRet += *(string++) - '0';
    }
    return toRet;
}

uint64_t printf(char* format, ...){
    va_list arg;
    uint64_t count = 0;
    va_start(arg, format);

    while(*format){ 
        if(*format == '%'){
            switch(*(format+1)){
                case 'd':
                case 'l':
                    count += printInt(va_arg(arg, uint64_t)); break;
                case 'X':
                case 'x':
                    count += printHex(va_arg(arg, uint64_t)); break;
                case 's':
                    count += print(va_arg(arg, char*)); break;
                default:
                    puts("Format not recognized");
                    return -1;
            }
            format += 2;
        } else {
            putChar(*(format++));
            count++;
        }
    }

    return count;
}

uint64_t printInt(uint64_t n){
    char buffer[20]; // uint64_t max = 18446744073709551615 (20 dígitos)
    int i = 0;

    // caso especial
    if (n == 0) {
        putChar('0');
        return 1;
    }

    // generar dígitos en orden inverso
    while (n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }

    uint64_t count = i;

    // imprimir en orden correcto
    while (i > 0) {
        putChar(buffer[--i]);
    }

    return count;
}

uint64_t printHex(uint64_t n){
    char buffer[16]; // como máximo 16 dígitos
    int i = 0;

    // caso especial
    if (n == 0) {
        putChar('0');
        return 1;
    }

    // generar dígitos en orden inverso
    while (n > 0) {
        if(n % 16 < 10)
            buffer[i++] = '0' + (n % 16);
        else
            buffer[i++] = 'A' + (n % 16) - 10;
        n /= 16;
    }

    uint64_t count = i;

    // imprimir en orden correcto
    while (i > 0) {
        putChar(buffer[--i]);
    }
    return count;
}

char lineBuffer[100];
uint64_t bufferIdx;

uint64_t scanf(char* format, ...){
    va_list arg;
    uint64_t count = 0;
    bufferIdx = 0;
    va_start(arg, format);
    readLine(lineBuffer, 100);

    while(*format){ 
        if(*format == '%'){
            switch(*(format+1)){
                case 'd':
                case 'l':
                    *(va_arg(arg, uint64_t*)) = readInt(); break;
                case 'X':
                case 'x':
                    *(va_arg(arg, uint64_t*)) = readHex(); break;
                case 's':
                    readString(va_arg(arg, char*)); break;
                default:
                    puts("Format not recognized");
                    return -1;
            }
            format += 2;
            count++;
            if(lineBuffer[--bufferIdx] == '\n' || lineBuffer[bufferIdx] == 0)
                break;
        } else if(*(format++) != lineBuffer[bufferIdx++]){
            break;
        } 
    }

    return count;
}

uint64_t readInt(){
    uint64_t result = 0;
    char c;

    while((c = lineBuffer[bufferIdx++])) {
        if (c >= '0' && c <= '9') {
            result = result * 10 + (uint64_t)(c - '0');
        } else {
            break;
        }
    }
    return result;

}

uint64_t readHex(){
    uint64_t result = 0;
    char c;
    int started = 0;

    while ((c = lineBuffer[bufferIdx++])) {
        if (!started && c == '0') {
            char n = lineBuffer[bufferIdx++];
            if (n == 'x' || n == 'X') {
                started = 1;
                continue;
            } else {
                bufferIdx -= 1;
                break;
            }
        }

        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            if (c >= '0' && c <= '9') result = (result << 4) + (uint64_t)(c - '0');
            else if (c >= 'a' && c <= 'f') result = (result << 4) + (uint64_t)(10 + c - 'a');
            else result = (result << 4) + (uint64_t)(10 + c - 'A');
        } else {
            break;
        }
    }
    
    return result;

}

uint64_t readString(char* string){
    int index = 0;
    char c;

    while ((c = lineBuffer[bufferIdx++])) {
        if (c == '\n' || c == ' ') {
            break;
        } else {
            string[index++] = c;
        }
    }
    string[index] = 0;
    return index;
}

void playBackgroundMelody(uint32_t* notes, uint64_t* durations, uint64_t count){
    sys_play_music(notes, durations, count);
}

void playMelody(uint32_t* notes, uint64_t* durations, uint64_t count){
    for(int i = 0; i < count; i++){
        sys_play_sound((uint64_t)notes[i], durations[i]);
    }
}