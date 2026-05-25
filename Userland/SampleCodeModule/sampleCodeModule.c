#include "include/libc.h"

#define BUFFER_SIZE 100

char line_buffer[BUFFER_SIZE+1];
char* shell_message = "@shell> ";

int strncontains(char* string, uint64_t size, char* expression){
    while(size--){
        if(*expression == 0)
            return 1;
        if(*(string++) != *(expression++))
            return 0;
    }
    return 0; 
}

void strtok(char* string, uint64_t size, char delim){
    while(*string){
        if(*string == delim){
            *string = 0;
        }
        string++;
    }
}

int main() {

    while(1){
        // set_text_color(0x30FF30); // Hay que hacer syscall de color
        print(shell_message);
        // set_text_color(0xFFFFFF); // lo mismo

        readLine(line_buffer, BUFFER_SIZE);
        strtok(line_buffer, BUFFER_SIZE, ' ');

        if(strncontains(line_buffer, BUFFER_SIZE, "time")){
            print("Es hora de programar\n");
            
        } else if(strncontains(line_buffer, BUFFER_SIZE, "pacman")){
            //print("work in progress\n");
            startPacman();
        } else if(strncontains(line_buffer, BUFFER_SIZE, "echo")){
            print(line_buffer + 5);
            print("\n");
            
        } else if(strncontains(line_buffer, BUFFER_SIZE, "redraw")){
            // redraw(); // Hay que hacer syscall de redraw
            print("Redraw no implementado aun\n");
            
        } else if(strncontains(line_buffer, BUFFER_SIZE, "exit")){
            return 0;
            
        } else if(strncontains(line_buffer, BUFFER_SIZE, "divzero")){
            print("Provocando division por cero...\n");
            volatile int a = 10;
            volatile int b = 0;
            volatile int c = a / b; 
            
        } else if(strncontains(line_buffer, BUFFER_SIZE, "invalidop")){
            print("Provocando Invalid Opcode...\n");
            __asm__("ud2");
        
        } else {
            print(line_buffer);
            print(" is not a command\n");
        }
    }

    return 0;
}