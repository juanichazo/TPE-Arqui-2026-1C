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

uint64_t hash(unsigned char *str){
    uint64_t hash = 0x1234;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

uint64_t printHash(char* params[]){
    printf("hash for %s: %d\n", params[0], hash(params[0]));
}

#define MAX_HASH_LENGTH 5

uint64_t bruteforce_hashcode(){
    char solution[MAX_HASH_LENGTH + 1] = {0};
    solution[0] = 'a';
    uint64_t time1 = reset_tsc();
    int found = 0;
    
    while(solution[MAX_HASH_LENGTH] == 0){
        if(sys_gethash() == hash(solution)){
            found = 1;
            break;
        }
        for(int i = 0; i <= MAX_HASH_LENGTH; i++){
            if(solution[i] == 0) solution[i] = 'a';
            else solution[i]++;
            if (solution[i] <= 'z') break;
            solution[i] = 'a';
        }
    }

    if(found)
        puts(solution);
    else
        puts("couldnt find solution");
    
    uint64_t time2 = reset_tsc();
    printf("%d cpu cycles\n", time2 - time1);
    
    return time2 - time1; 
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
    uint64_t size = string_to_int(params[0]);
    if(!sys_settextsize(size)){
        printf("%d is not a valid size\n", size);
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
                
            printf("%s is not a command\n", line_buffer);
        }
    }
    return 0;
}