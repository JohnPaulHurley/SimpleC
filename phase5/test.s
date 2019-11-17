_g:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.globl	_g

_f:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	, -4(%rbp)
	movl	, -8(%rbp)
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.globl	_f

_main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movl	$500, -49(%rbp)
	movl	, _p1(%rip)
	movl	, -12(%rbp)
	movl	, _q1(%rip)
	movl	, -28(%rbp)
	movl	, _s1(%rip)
	movl	, -37(%rbp)
	movq	%rbp, %rsp
	popq	%rbp
	ret

	.globl	_main

	.comm	_i1, 4
	.comm	_p1, 8
	.comm	_z, 4
	.comm	_l1, 8
	.comm	_q1, 8
	.comm	_c1, 1
	.comm	_s1, 8
	.comm	_s3, 8
	.comm	_s, 100
	.comm	_t, 100
