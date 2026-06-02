#include <kernelCracker.h>


uint64_t hash(char *str){
    uint64_t hash = 0x1234;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

void printHash(char* params[]){
    printf("hash for %s: %d\n", params[0], hash_asm(params[0]));
}

#define MAX_HASH_LENGTH 5

uint64_t bruteforce_hashcode(){
    char solution[MAX_HASH_LENGTH + 1] = {'z', 'z', 'z', 'd', 0};
    solution[0] = 'a';
    int found = 0;
    char* end = &solution[MAX_HASH_LENGTH];
    uint64_t time1, time2;

    time1 = reset_tsc();
    while(*end == 0 && sys_gethash() != hash_asm(solution)){
        for(int i = 0; i <= MAX_HASH_LENGTH; i++){
            if(solution[i] == 0) solution[i] = 'a';
            else solution[i]++;
            if (solution[i] <= 'z') break;
            solution[i] = 'a';
        }   
    }
    time2 = reset_tsc();
    uint64_t total_cycles = time2 - time1;

    if(*end == 0)
        puts(solution);
    else
        puts("couldnt find solution");
    
    printf("%d cpu cycles\n", total_cycles);
    uint64_t cpu_timer1 = reset_tsc();
    sleep(20);
    uint64_t cycles_per_ms = (reset_tsc() - cpu_timer1) / 1000;
    printf("%d ms\n", total_cycles /  cycles_per_ms);

    return total_cycles; 
}

void sethash(char* params[]){
    sys_sethash(string_to_int(params[0]));
}