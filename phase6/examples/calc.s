	.file	"calc.c"
	.comm	ADD,4,4
	.comm	SUB,4,4
	.comm	MUL,4,4
	.comm	DIV,4,4
	.comm	NUM,4,4
	.comm	LF,4,4
	.comm	LPAREN,4,4
	.comm	RPAREN,4,4
	.comm	lookahead,4,4
	.comm	c,4,4
	.comm	lexval,4,4
	.text
.globl lexan
	.type	lexan, @function
lexan:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	c(%rip), %eax
	testl	%eax, %eax
	jne	.L11
	movl	$0, %eax
	call	getchar
	movl	%eax, c(%rip)
	jmp	.L3
.L5:
	movl	$0, %eax
	call	getchar
	movl	%eax, c(%rip)
	jmp	.L3
.L11:
	nop
.L3:
	movl	c(%rip), %eax
	movl	%eax, %edi
	call	isspace
	testl	%eax, %eax
	je	.L4
	movl	c(%rip), %edx
	movl	LF(%rip), %eax
	cmpl	%eax, %edx
	jne	.L5
.L4:
	movl	c(%rip), %eax
	subl	$48, %eax
	cmpl	$9, %eax
	jbe	.L6
	movl	c(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	$0, c(%rip)
	movl	-4(%rbp), %eax
	jmp	.L7
.L6:
	movl	$0, -8(%rbp)
	jmp	.L8
.L9:
	movl	-8(%rbp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	movl	%eax, %edx
	movl	c(%rip), %eax
	leal	(%rdx,%rax), %eax
	subl	$48, %eax
	movl	%eax, -8(%rbp)
	movl	c(%rip), %eax
	movl	%eax, %edi
	movl	$0, %eax
	call	getchar
	movl	%eax, c(%rip)
.L8:
	movl	c(%rip), %eax
	subl	$48, %eax
	cmpl	$9, %eax
	jbe	.L9
	movl	-8(%rbp), %eax
	movl	%eax, lexval(%rip)
	movl	NUM(%rip), %eax
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	lexan, .-lexan
	.section	.rodata
.LC0:
	.string	"syntax error at %d\n"
	.text
.globl match
	.type	match, @function
match:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	lookahead(%rip), %eax
	cmpl	-4(%rbp), %eax
	je	.L13
	movl	lookahead(%rip), %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$1, %edi
	call	exit
.L13:
	call	lexan
	movl	%eax, lookahead(%rip)
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	match, .-match
.globl factor
	.type	factor, @function
factor:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	lookahead(%rip), %edx
	movl	LPAREN(%rip), %eax
	cmpl	%eax, %edx
	jne	.L16
	movl	LPAREN(%rip), %eax
	movl	%eax, %edi
	call	match
	call	expr
	movl	%eax, -4(%rbp)
	movl	RPAREN(%rip), %eax
	movl	%eax, %edi
	call	match
	movl	-4(%rbp), %eax
	jmp	.L17
.L16:
	movl	lexval(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	NUM(%rip), %eax
	movl	%eax, %edi
	call	match
	movl	-4(%rbp), %eax
.L17:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	factor, .-factor
.globl term
	.type	term, @function
term:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	call	factor
	movl	%eax, -4(%rbp)
	jmp	.L20
.L22:
	movl	lookahead(%rip), %edx
	movl	MUL(%rip), %eax
	cmpl	%eax, %edx
	jne	.L21
	movl	MUL(%rip), %eax
	movl	%eax, %edi
	call	match
	call	factor
	movl	-4(%rbp), %edx
	imull	%edx, %eax
	movl	%eax, -4(%rbp)
	jmp	.L20
.L21:
	movl	DIV(%rip), %eax
	movl	%eax, %edi
	call	match
	call	factor
	movl	%eax, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	sarl	$31, %edx
	idivl	-20(%rbp)
	movl	%eax, -4(%rbp)
.L20:
	movl	lookahead(%rip), %edx
	movl	MUL(%rip), %eax
	cmpl	%eax, %edx
	je	.L22
	movl	lookahead(%rip), %edx
	movl	DIV(%rip), %eax
	cmpl	%eax, %edx
	je	.L22
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	term, .-term
.globl expr
	.type	expr, @function
expr:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	call	term
	movl	%eax, -4(%rbp)
	jmp	.L25
.L27:
	movl	lookahead(%rip), %edx
	movl	ADD(%rip), %eax
	cmpl	%eax, %edx
	jne	.L26
	movl	ADD(%rip), %eax
	movl	%eax, %edi
	call	match
	call	term
	addl	%eax, -4(%rbp)
	jmp	.L25
.L26:
	movl	SUB(%rip), %eax
	movl	%eax, %edi
	call	match
	call	term
	subl	%eax, -4(%rbp)
.L25:
	movl	lookahead(%rip), %edx
	movl	ADD(%rip), %eax
	cmpl	%eax, %edx
	je	.L27
	movl	lookahead(%rip), %edx
	movl	SUB(%rip), %eax
	cmpl	%eax, %edx
	je	.L27
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	expr, .-expr
	.section	.rodata
.LC1:
	.string	"%d\n"
	.text
.globl main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$10, LF(%rip)
	movl	$40, LPAREN(%rip)
	movl	$41, RPAREN(%rip)
	movl	$42, MUL(%rip)
	movl	$43, ADD(%rip)
	movl	$45, SUB(%rip)
	movl	$47, DIV(%rip)
	movl	$256, NUM(%rip)
	call	lexan
	movl	%eax, lookahead(%rip)
	jmp	.L30
.L33:
	call	expr
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	jmp	.L31
.L32:
	movl	LF(%rip), %eax
	movl	%eax, %edi
	call	match
.L31:
	movl	lookahead(%rip), %edx
	movl	LF(%rip), %eax
	cmpl	%eax, %edx
	je	.L32
.L30:
	movl	lookahead(%rip), %eax
	cmpl	$-1, %eax
	jne	.L33
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.7 20120313 (Red Hat 4.4.7-17)"
	.section	.note.GNU-stack,"",@progbits
