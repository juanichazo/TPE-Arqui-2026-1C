GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_time
GLOBAL sys_draw
GLOBAL sys_sethash
GLOBAL sys_gethash
GLOBAL sys_setcolor
GLOBAL sys_settextsize
GLOBAL sys_sleep
GLOBAL reset_tsc

section .text

%macro syscall 1
    push rbp
    mov rbp, rsp

    mov rax, %1      ; syscall number
    int 80h

    mov rsp, rbp
    pop rbp
    ret
%endmacro


; uint64_t sys_read(uint64_t fd, char * buffer, uint64_t count)
sys_read:
    syscall 0

; uint64_t sys_write(uint64_t fd, char* buffer, uint64_t count)
sys_write:
    syscall 1

; uint64_t sys_time(uint64_t selector)
sys_time:
    syscall 2

;uint64_t sys_draw(uint32_t color, uint64_t x, uint64_t y)
sys_draw:
    syscall 3

sys_sethash:
    syscall 4
    
sys_gethash:
    syscall 5

sys_setcolor:
    syscall 6

sys_settextsize:
    syscall 7

sys_sleep:
    syscall 8

reset_tsc: ; TODO esta función habría que moverla a otro lado probablemente
    xor rax, rax
    xor rdx, rdx
    rdtsc
    shl rdx, 32
    or rax, rdx
    ret

