#include <libc.h>
#include <user_syscalls.h>
#include <pacman.h>
#include <kernelCracker.h>

#define BUFFER_SIZE 255

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

void echo(char* params[0]){
    while(*params){
        printf("%s ", *(params++));
    }
    putChar('\n');
}

void clear(){
    char clear[2] = {14, 0};
    printf(clear);
}

void setsize(char* params[]){
    uint64_t size = string_to_int(params[0]);
    if(sys_settextsize(size) == 1){ // FAILURE definido con 1
        printf("%d no es un tamano valido\n", size);
    }
}

void divzero(){
    printf("Provocando division por cero...\n");
    volatile int a = 10;
    volatile int b = 0;
    volatile int c = a / b; 
}

void invalidop(){
    printf("Provocando Invalid Opcode...\n");
    __asm__("ud2");
}

typedef int (*CommandFunction)(char* params[]);

typedef struct{
    char* name;
    CommandFunction function;
} Command;

Command commands[] = {
    {"help",    (CommandFunction) help},
    {"time",    (CommandFunction) getTime}, 
    {"pacman",  (CommandFunction) startPacman},
    {"echo",    (CommandFunction) echo},
    {"divzero", (CommandFunction) divzero},
    {"invalidop", (CommandFunction) invalidop},
    {"bruteforce", (CommandFunction) bruteforce_hashcode},
    {"sethash", (CommandFunction) sethash},
    {"hash",    (CommandFunction) printHash},
    {"clear",   (CommandFunction) clear},
    {"size",    (CommandFunction) setsize},
    {"regs",    (CommandFunction) printRegisters}
};

void runShell(){
    int is_command;
    while(1){
        sys_setcolor(0x30FF30, 0x0);
        printf(shell_message);
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
                return;
                
            printf("%s no es un comando\n", line_buffer);
        }
    }
}