#include <libc.h>
#include <user_syscalls.h>
#include <pacman.h>

#define BUFFER_SIZE 100

char line_buffer[BUFFER_SIZE+1];
char* params[BUFFER_SIZE+1];
char* shell_message = "@shell> ";

void help(){
    puts("Comandos disponibles:");
    puts(" help               : Muestra esta ayuda");
    puts(" time               : Muestra la hora del sistema");
    puts(" pacman             : Inicia el juego Pacman");
    puts(" echo               : Muestra <texto> en pantalla");
    puts(" divzero            : Provoca una division por cero (error)");
    puts(" invalidop          : Provoca un error por opcode invalido");
    puts(" bruteforce         : Busca string cuyo hash coincide");
    puts(" sethash            : Establece el valor de hash del sistema");
    puts(" clear              : Limpia la pantalla");
    puts(" size               : Cambia el tamano del texto");
    puts(" regs               : Muestra registros de la CPU");
}

uint64_t bruteforce_hashcode(){
    char solution[6] = {0};
    solution[0] = 'a';

    uint64_t time1 = reset_tsc();
    
    while(solution[5] == 0){
        if(sys_gethash() == hash(solution)){
            puts(solution);
            uint64_t time2 = reset_tsc();
            print(int_to_str(time2 - time1, "               \0"));
            print(" cpu cycles\n");
            return time2 - time1;
        }
        for(int i = 0; i < 6; i++){
            if(solution[i] == 0) solution[i] = 'a';
            else solution[i]++;
            if (solution[i] <= 'z') break;
            solution[i] = 'a';
        }
    }

    puts("couldnt find solution");
    
    return reset_tsc() - time1; 
}

uint64_t hash(unsigned char *str){
    uint64_t hash = 0x1234;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

void sethash(char* params[]){
    sys_sethash(string_to_int(params[0]));
}

void echo(){
    puts(line_buffer + 5);
}

void clear(){
    char clear[2] = {14, 0};
    print(clear);
}

void setsize(char* params[]){
    if(sys_settextsize(string_to_int(params[0]))){
        print(params[0]);
        print(" is not a valid size");
    }
}

void divzero(){
    print("Provocando division por cero...'\n'");
    volatile int a = 10;
    volatile int b = 0;
    volatile int c = a / b; 
}

void invalidop(){
    print("Provocando Invalid Opcode...\n");
    __asm__("ud2");
}

typedef struct{
    char* name;
    int (*function)(char* params[]);
} Command;

Command commands[] = {
    {"help", help},
    {"time", getTime}, 
    {"pacman", startPacman},
    {"echo", echo},
    {"divzero", divzero},
    {"invalidop", invalidop},
    {"bruteforce", bruteforce_hashcode},
    {"sethash", sethash},
    {"clear", clear},
    {"size", setsize},
    {"regs", printRegisters}
};

int main() {
    int is_command;
    while(1){
        sys_setcolor(0x30FF30, 0x0);
        print(shell_message);
        sys_setcolor(0xFFFFFF, 0x0);
        
        readLine(line_buffer, BUFFER_SIZE);
        strtok(line_buffer, BUFFER_SIZE, ' ', params);

        is_command = 0;
        for(int i = 0; i < sizeof(commands) / sizeof(Command) && !is_command; i++){
            if(strncontains(line_buffer, BUFFER_SIZE, commands[i].name)){
                is_command = 1;
                commands[i].function(params);
            }    
        }
        if(!is_command){
            if(strncontains(line_buffer, BUFFER_SIZE, "exit"))
                return 0;

            print(line_buffer);
            print(" is not a command\n");
        }
    }

    return 0;
}