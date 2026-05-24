GLOBAL sys_write
GLOBAL sys_time

section .text

; uint64_t sys_write(uint64_t fd, char* buffer, uint64_t count)
sys_write:
    push rbp
    mov rbp, rsp

    mov rax, 1      ; 1 es el código para WRITE
    int 80h

    mov rsp, rbp
    pop rbp
    ret

; uint64_t sys_time(uint64_t selector)
sys_time:
    push rbp
    mov rbp, rsp

    mov rax, 2      ; 2 es el código para TIME
    int 80h

    mov rsp, rbp
    pop rbp
    ret