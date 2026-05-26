#include <libc.h>
#include <user_syscalls.h>
#include <pacman.h>

#define BUFFER_SIZE 100

char line_buffer[BUFFER_SIZE+1];
char* params[BUFFER_SIZE+1];
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
    int current = 0;
    while(*string){
        if(*string == delim){
            if(*(string+1) != 0 && current < BUFFER_SIZE) params[current++] = string+1;
            *string = 0;
        }
        string++;
    }
}

uint64_t bruteforce_hashcode(char* string){
    uint64_t hash = sys_gethash(string);

    char solution[6] = {0};
    solution[0] = 'a';

    reset_tsc();

    while(solution[5] == 0){
        if(sys_gethash(solution) == hash){
            puts(solution);
            return reset_tsc();
        }
        for(int i = 0; i < 6; i++){
            if(solution[i] == 0) solution[i] = 'a';
            else solution[i]++;
            if (solution[i] <= 'z') break;
            solution[i] = 'a';
        }
    }

    puts("couldnt find solution");
    return reset_tsc(); 
}

uint64_t string_to_int(char* string){
    uint64_t toRet = 0;
    while(*string >= '0' && *string <= '9'){
        toRet *= 10;
        toRet = *(string++) - '0';
    }
    return toRet;
}

char* int_to_str(uint64_t num, char* string){
    int i = 0;
    while(num > 0){
        string[i++] = (char)(num % 10) + '0';
        num /= 10;
    }
    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = string[j];
        string[j] = string[k];
        string[k] = temp;
    }

    return string;
}

int main() {
    int size = 10;
    while(1){
        sys_setcolor(0x30FF30, 0x0);
        print(shell_message);
        sys_setcolor(0xFFFFFF, 0x0);
        
        readLine(line_buffer, BUFFER_SIZE);
        strtok(line_buffer, BUFFER_SIZE, ' ');

        if(strncontains(line_buffer, BUFFER_SIZE, "time")){
            getTime();
        } else if(strncontains(line_buffer, BUFFER_SIZE, "pacman")){
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
        
        } else if(strncontains(line_buffer, BUFFER_SIZE, "bruteforce")){
            uint64_t time = bruteforce_hashcode(params[0]);
            print(int_to_str(time, "               \0"));
            print(" cpu cycles\n");
        } else if(strncontains(line_buffer, BUFFER_SIZE, "sethash")){
            sys_sethash(string_to_int(params[0]));
        } else if(strncontains(line_buffer, BUFFER_SIZE, "clear")){
            char clear[2] = {14,0};
            print(clear);
        } else if(strncontains(line_buffer, BUFFER_SIZE, "smaller")){
            puts("Size changed: ");
            if(!sys_settextsize(--size))
                print(int_to_str(size, "      \n"));
            else 
                print("failed\n");
        } else if(strncontains(line_buffer, BUFFER_SIZE, "bigger")){
            puts("Size changed: ");
            if(!sys_settextsize(++size))
                print(int_to_str(size, "       \n"));
            else 
                print("failed\n");
        } else {
            print(line_buffer);
            print(" is not a command\n");
        }
    }

    return 0;
}