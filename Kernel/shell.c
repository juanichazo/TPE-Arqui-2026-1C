
#include <console.h>

#define BUFFER_SIZE 100

char line_buffer[BUFFER_SIZE];

char* shell_message = "@shell> ";

int runShell(){

    while(1){
        set_text_color(0x30FF30);
        print(shell_message);
        set_text_color(0xFFFFFF);

        readLine(line_buffer, BUFFER_SIZE);
        if(strncontains(line_buffer, BUFFER_SIZE, "time")){
            puts("Es hora de programar");
        } else if(strncontains(line_buffer, BUFFER_SIZE, "pacman")){
            puts("work in progress");
        } else if(strncontains(line_buffer, BUFFER_SIZE, "echo")){
            puts(line_buffer + 5);
        } else if(strncontains(line_buffer, BUFFER_SIZE, "exit")){
            return 0;
        } else {
            print(line_buffer);
            puts(" is not a command");
        }
    }

    return 0;
}

int strncontains(char* string, uint64_t size, char* expression){
    while(size--){
        if(*expression == 0)
            return 1;
        if(*(string++) != *(expression++))
            return 0;
    }
}