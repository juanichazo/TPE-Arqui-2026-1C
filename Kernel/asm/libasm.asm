GLOBAL cpuVendor
GLOBAL readKeyboard
GLOBAL inb
GLOBAL outb

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

inb:
    push rbp
    mov rbp, rsp
    mov dx, di
    in al, dx
    movzx rax, al
    mov rsp, rbp
    pop rbp
    ret

outb:
    push rbp
    mov rbp, rsp
    mov dx, di
    mov al, sil
    out dx, al
    mov rsp, rbp
    pop rbp
    ret

readKeyboard:
	xor rax, rax
	in al, 60h
	movzx rax, al
	ret