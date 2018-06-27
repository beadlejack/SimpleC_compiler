	.file	"putchar.c"
	.text
.globl main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$83, %edi
	call	putchar
	movl	$101, %edi
	call	putchar
	movl	$103, %edi
	call	putchar
	movl	$109, %edi
	call	putchar
	movl	$101, %edi
	call	putchar
	movl	$110, %edi
	call	putchar
	movl	$116, %edi
	call	putchar
	movl	$97, %edi
	call	putchar
	movl	$116, %edi
	call	putchar
	movl	$105, %edi
	call	putchar
	movl	$111, %edi
	call	putchar
	movl	$110, %edi
	call	putchar
	movl	$32, %edi
	call	putchar
	movl	$102, %edi
	call	putchar
	movl	$97, %edi
	call	putchar
	movl	$117, %edi
	call	putchar
	movl	$108, %edi
	call	putchar
	movl	$116, %edi
	call	putchar
	movl	$10, %edi
	call	putchar
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-17)"
	.section	.note.GNU-stack,"",@progbits
