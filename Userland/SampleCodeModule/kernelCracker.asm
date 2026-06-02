GLOBAL reset_tsc
GLOBAL hash_asm
GLOBAL get_next

reset_tsc: ; TODO esta función habría que moverla a otro lado probablemente
    xor rax, rax
    xor rdx, rdx
    rdtsc
    shl rdx, 32
    or rax, rdx
    ret

hash_asm:
    movsx   rdx, BYTE[rdi]
    test    dl, dl
    je      .L4
    add     rdi, 1
    mov     eax, 0x1234
.L3:
    mov     rcx, rax
    add     rdi, 1
    sal     rcx, 5
    add     rax, rcx
    add     rax, rdx
    movsx   rdx, BYTE[rdi-1]
    test    dl, dl
    jne     .L3
    ret
.L4:
    mov     eax, 0x1234
    ret