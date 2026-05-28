GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_time
GLOBAL sys_draw
GLOBAL sys_sethash
GLOBAL sys_gethash
GLOBAL sys_setcolor
GLOBAL sys_settextsize
GLOBAL sys_sleep
GLOBAL sys_draw_rect
GLOBAL sys_draw_buffer
GLOBAL reset_tsc
GLOBAL get_registers

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

sys_draw_rect
    syscall 9

sys_draw_buffer
    syscall 10

reset_tsc: ; TODO esta función habría que moverla a otro lado probablemente
    xor rax, rax
    xor rdx, rdx
    rdtsc
    ;shl rdx, 32
    ;or rax, rdx
    ret


get_registers:
    ; rdi tiene la direc del arreglo
    mov [rdi + 8], rbx 
    mov [rdi + 8*2], rcx 
    mov [rdi + 8*3], rdx 
    mov [rdi + 8*4], rsi
    mov [rdi + 8*5], rdi
    mov [rdi + 8*6], rbp
    mov [rdi + 8*7], r8
    mov [rdi + 8*8], r9
    mov [rdi + 8*9], r10
    mov [rdi + 8*10], r11
    mov [rdi + 8*11], r12
    mov [rdi + 8*12], r13
    mov [rdi + 8*13], r14
    mov [rdi + 8*14], r15

    ; extraemos registros del stack
    mov rbx, [rsp]          ; RIP
    mov [rdi + 8*15], rbx 
    
    mov rbx, [rsp + 8]      ; CS
    mov [rdi + 8*16], rbx
    
    mov rbx, [rsp + 16]     ; RFLAGS
    mov [rdi + 8*17], rbx 
    
    mov rbx, [rsp + 24]     ; rsp de userland 
    mov [rdi + 8*18], rbx

    ret
