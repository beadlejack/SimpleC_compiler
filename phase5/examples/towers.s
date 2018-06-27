	.file	"towers.c"
	.text
.globl towers
	.type	towers, @function
towers:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	%ecx, -32(%rbp)
	movl	-28(%rbp), %ecx
	movl	-32(%rbp), %edx
	movl	-24(%rbp), %ebx
	.cfi_offset 3, -24
	movl	-20(%rbp), %eax
	movl	%ebx, %esi
	movl	%eax, %edi
	call	call_towers
	movl	-28(%rbp), %edx
	movl	-24(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	print_move
	movl	-24(%rbp), %ecx
	movl	-28(%rbp), %edx
	movl	-32(%rbp), %ebx
	movl	-20(%rbp), %eax
	movl	%ebx, %esi
	movl	%eax, %edi
	call	call_towers
	addq	$24, %rsp
	popq	%rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	towers, .-towers
.globl main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$3, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print
	movl	-4(%rbp), %eax
	movl	$3, %ecx
	movl	$2, %edx
	movl	$1, %esi
	movl	%eax, %edi
	call	towers
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-17)"
	.section	.note.GNU-stack,"",@progbits
