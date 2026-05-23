GLOBAL _exception00Handler
GLOBAL _exception06Handler
EXTERN exceptionDispatcher
EXTERN getStackBase

section .text

_exception00Handler:
    mov [regs], rax         ; rax en la posición 0 del arreglo
    mov rax, 0              ; id de la excepción (0 = División por cero) 
    jmp exceptionMasterHandler

_exception06Handler:
    mov [regs], rax         ; rax en la posición 0 
    mov rax, 6              ; id de la excepción (6 = Invalid Opcode) 
    jmp exceptionMasterHandler

exceptionMasterHandler:
    ; guardo los registros de proposito general
    mov [regs + 8], rbx 
    mov [regs + 8*2], rcx 
    mov [regs + 8*3], rdx 
    mov [regs + 8*4], rsi
    mov [regs + 8*5], rdi
    mov [regs + 8*6], rbp
    mov [regs + 8*7], r8
    mov [regs + 8*8], r9
    mov [regs + 8*9], r10
    mov [regs + 8*10], r11
    mov [regs + 8*11], r12
    mov [regs + 8*12], r13
    mov [regs + 8*13], r14
    mov [regs + 8*14], r15

    ; extraemos registros del stack
    mov rbx, [rsp]          ; RIP
    mov [regs + 8*15], rbx 
    
    mov rbx, [rsp + 8]      ; CS
    mov [regs + 8*16], rbx
    
    mov rbx, [rsp + 16]     ; RFLAGS
    mov [regs + 8*17], rbx 
    
    mov rbx, [rsp + 24]     ; rsp de userland 
    mov [regs + 8*18], rbx

    ; mostrar todo en pantalla 
    mov rdi, rax              ; paso el primer argumento (id de la excepcion)
    mov rsi, regs             ; paso el segundo argumento (arreglo de registros)
    call exceptionDispatcher

    ; recuperación
    mov rcx, [rsp + 8]        ; CS
    mov rdx, [rsp + 16]       ; RFLAGS
    call getStackBase         ; lo guarda en rax
    mov rsp, rax
    push rdx                  ; 3ro que va a salir: RFLAGS
    push rcx                  ; 2do que va a salir: CS
    push 0x400000             ; 1ro que va a salir: RIP (Inicio de Userland/Shell)
    call getStackBase         ; queda en rax 
    iretq                     ; popea todo de la pila

section .data
REGS_AMOUNT equ 19

section .bss
regs resq REGS_AMOUNT