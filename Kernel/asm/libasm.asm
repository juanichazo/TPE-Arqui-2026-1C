GLOBAL cpuVendor
GLOBAL readKeyboard
GLOBAL inb
GLOBAL outb
GLOBAL tsc

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
    mov rdx, rdi
	xor rax, rax
    in al, dx
    ret

outb:
    mov dx, di
    mov al, sil
    out dx, al
    ret

readKeyboard:
	xor rax, rax
	in al, 60h
	movzx rax, al
	ret

tsc: 
    xor rax, rax
    xor rdx, rdx
    rdtsc
    shl rdx, 32
    or rax, rdx
    ret