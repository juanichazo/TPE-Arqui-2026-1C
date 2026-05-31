GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL getregs

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception00Handler
GLOBAL _exception06Handler
EXTERN exceptionDispatcher
EXTERN getStackBase
GLOBAL _syscall80Handler
GLOBAL snapshot_regs

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscallDispatcher

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro snapshot_regs 0
; guardo los registros de proposito general
	mov [regs], rax
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

	push rax ; para preservarlo

    ; extraemos registros del stack
    mov rax, [rsp + 8]          ; RIP
    mov [regs + 8*15], rax 
    
    mov rax, [rsp + 16]      ; CS
    mov [regs + 8*16], rax
    
    mov rax, [rsp + 24]     ; RFLAGS
    mov [regs + 8*17], rax 
    
    mov rax, [rsp + 32]     ; rsp de userland 
    mov [regs + 8*18], rax

	mov rax, [rsp + 40]		; SS
	mov [regs + 8*19], rax

	pop rax
%endmacro

%macro irqHandlerMaster 1
	snapshot_regs
	pushState
	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	popState
	iretq
%endmacro

%macro syscallHandler 0
	pushState

	; Movemos el número de syscall (saved rax) a rdi
	mov rdi, [rsp + 112]
	
	; Movemos el primer parámetro (saved rdi) a rsi
	mov rsi, [rsp + 72]
	
	; Movemos el segundo parámetro (saved rsi) a rdx
	mov rdx, [rsp + 64]
	
	; Movemos el tercer parámetro (saved rdx) a rcx
	mov rcx, [rsp + 88]

	; Movemos el cuarto parámetro (saved rcx) a r8 ---
	mov r8, [rsp + 96]

	; Movemos el quinto parámetro (saved r8) a r9 ---
	mov r9, [rsp + 56]

	call syscallDispatcher

	; Pisamos el RAX guardado en el stack con el valor de retorno de la syscall
	mov [rsp + 112], rax

	popState
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


; Software interrupt - Syscall (int 0x80)
_syscall80Handler:
	syscallHandler

_exception00Handler: ; estos dos pueden hacerse con una macro
	snapshot_regs
    mov rax, 0              ; id de la excepción (0 = División por cero) 
    jmp exceptionMasterHandler

_exception06Handler:
	snapshot_regs
    mov rax, 6              ; id de la excepción (6 = Invalid Opcode) 
    jmp exceptionMasterHandler

exceptionMasterHandler:

    ; mostrar todo en pantalla 
    mov rdi, rax              ; paso el primer argumento (id de la excepcion)
    mov rsi, regs             ; paso el segundo argumento (arreglo de registros)
    call exceptionDispatcher

    ; recuperación
    mov rcx, [regs + 8*16]    ; CS
    mov rdx, [regs + 8*17]    ; RFLAGS
	mov rbx, [regs + 8*19]	  ; SS
    call getStackBase         ; lo guarda en rax
    mov rsp, rax
	push rbx				  ; 5to que sale: SS
	push rax				  ; 4to que sale: RSP
    push rdx                  ; 3ro que va a salir: RFLAGS
    push rcx                  ; 2do que va a salir: CS
    push 0x400000             ; 1ro que va a salir: RIP (Inicio de Userland/Shell)
    iretq                     ; popea todo de la pila

haltcpu:
	cli
	hlt
	ret

getregs:
	mov rax, regs
	ret

SECTION .data
REGS_AMOUNT equ 20

SECTION .bss
aux resq 1
regs resq REGS_AMOUNT