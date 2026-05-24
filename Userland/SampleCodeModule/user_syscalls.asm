GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_time
GLOBAL sys_draw

section .text

; uint64_t sys_read(uint64_t fd, char * buffer, uint64_t count)
sys_read:
    push rbp
    mov rbp, rsp

    mov rax, 0      ; read
    int 80h

    mov rsp, rbp
    pop rbp
    ret

; uint64_t sys_write(uint64_t fd, char* buffer, uint64_t count)
sys_write:
    push rbp
    mov rbp, rsp

    mov rax, 1      ; write
    int 80h

    mov rsp, rbp
    pop rbp
    ret

; uint64_t sys_time(uint64_t selector)
sys_time:
    push rbp
    mov rbp, rsp

    mov rax, 2      ;  time
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;uint64_t sys_draw(uint32_t color, uint64_t x, uint64_t y)
sys_draw:
    push rbp
    mov rbp, rsp

    mov rax, 3      ; draw
    int 80h

    mov rsp, rbp
    pop rbp
    ret
