_g:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$g.size, %eax
	subq	%rax, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %edi
	movl	$0, %eax
	call	_abs
	mov	%eax, -8(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	mov	%eax, -12(%rbp)	# spill
	movl	-12(%rbp), %edi
	movl	$0, %eax
	call	_abs
	addl	$1,%eax
	mov	%eax, -16(%rbp)	# spill
	movl	-8(%rbp), %eax
	movl	-16(%rbp), %r11d
	cltd	
	idivl	%r11d
	movl	%edx, %eax
	jmp	.L0
.L0:
	movq	%rbp, %rsp
	popq	%rbp
	ret
	.set	g.size, 16
	.globl	_g

_f:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$f.size, %eax
	subq	%rax, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	$100000, %r11d
	cltd	
	idivl	%r11d
	movl	%edx, -4(%rbp)
	movl	-8(%rbp), %eax
	movl	$100000, %r11d
	cltd	
	idivl	%r11d
	movl	%edx, -8(%rbp)
	lea	_s(%rip),%r11
	movq	$100, %r10
	imulq	$1,%r10
	addq	%r10,%r11
	movl	$1, %r10d
	movslq	%r10d,%r10
	imulq	$1,%r10
	subq	%r10,%r11
	movq	_s3(%rip), %r10
	cmpl	%r11d,%r10d
	setl	%r11b
	movzbl	%r11b,%r10d
	cmp	$0,%r10d
	je	.L3
	movl	$102, %r11d
	movq	_s3(%rip), %r9
	movb	%r11b, (%r9)
	movl	$1, %r8d
	movslq	%r8d,%r8
	imulq	$1,%r8
	movq	_s3(%rip), %rcx
	addq	%r8,%rcx
	movq	%rcx, _s3(%rip)
	jmp	.L3
.L3:
	movl	-8(%rbp), %r10d
	imull	$2,%r10d
	movl	-4(%rbp), %r8d
	cmpl	%r10d,%r8d
	setge	%r10b
	movzbl	%r10b,%r8d
	cmp	$0,%r8d
	je	.L4
	movl	-8(%rbp), %r10d
	imull	$3,%r10d
	movl	-4(%rbp), %ecx
	subl	%r10d,%ecx
	movl	%ecx, %eax
	jmp	.L1
	jmp	.L5
.L4:
	movl	-4(%rbp), %eax
	movl	$2, %r10d
	cltd	
	idivl	%r10d
	movl	$0, %r10d
	cmpl	%r10d,%edx
	sete	%r10b
	movzbl	%r10b,%edx
	cmp	$0,%edx
	je	.L6
	movl	-4(%rbp), %r10d
	imull	$4,%r10d
	movl	-8(%rbp), %eax
	movl	$3, %ecx
	cltd	
	idivl	%ecx
	mov	%r11b, -9(%rbp)	# spill
	mov	%r10d, -13(%rbp)	# spill
	mov	%r9, -21(%rbp)	# spill
	mov	%r8d, -25(%rbp)	# spill
	mov	%edx, -29(%rbp)	# spill
	mov	%eax, -33(%rbp)	# spill
	movl	-33(%rbp), %esi
	movl	-13(%rbp), %edi
	call	_f
	movl	-4(%rbp), %r11d
	addl	$100,%r11d
	movl	-8(%rbp), %r10d
	subl	$4,%r10d
	mov	%r11d, -37(%rbp)	# spill
	mov	%r10d, -41(%rbp)	# spill
	mov	%eax, -45(%rbp)	# spill
	movl	-41(%rbp), %esi
	movl	-37(%rbp), %edi
	call	_f
	movl	-45(%rbp), %r11d
	addl	%eax,%r11d
	movl	%r11d, %eax
	jmp	.L1
	jmp	.L7
.L6:
	movl	-4(%rbp), %r11d
	imull	$2,%r11d
	movl	-8(%rbp), %eax
	movl	$3, %r10d
	cltd	
	idivl	%r10d
	mov	%r11d, -49(%rbp)	# spill
	mov	%eax, -53(%rbp)	# spill
	movl	-53(%rbp), %esi
	movl	-49(%rbp), %edi
	call	_f
	mov	%eax, -57(%rbp)	# spill
	movl	-4(%rbp), %edi
	call	_g
	mov	%eax, -61(%rbp)	# spill
	movl	-61(%rbp), %edi
	call	_g
	movl	-4(%rbp), %r11d
	addl	%eax,%r11d
	movl	-8(%rbp), %r10d
	subl	$10,%r10d
	mov	%r11d, -65(%rbp)	# spill
	mov	%r10d, -69(%rbp)	# spill
	movl	-69(%rbp), %esi
	movl	-65(%rbp), %edi
	call	_f
	movl	-57(%rbp), %r11d
	addl	%eax,%r11d
	movl	%r11d, %eax
	jmp	.L1
.L7:
.L5:
.L1:
	movq	%rbp, %rsp
	popq	%rbp
	ret
	.set	f.size, 80
	.globl	_f

_main:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	$main.size, %eax
	subq	%rax, %rsp
	movl	$500, %r11d
	movl	%r11d, -49(%rbp)
	lea	-4(%rbp),%r11
	movq	%r11, _p1(%rip)
	lea	_i1(%rip),%r11
	movq	%r11, -12(%rbp)
	lea	-20(%rbp),%r11
	movq	%r11, _q1(%rip)
	lea	_l1(%rip),%r11
	movq	%r11, -28(%rbp)
	lea	-29(%rbp),%r11
	movq	%r11, _s1(%rip)
	lea	_c1(%rip),%r11
	movq	%r11, -37(%rbp)
.L9:
	movl	-49(%rbp), %r11d
	movl	$0, %r10d
	cmpl	%r10d,%r11d
	setg	%r10b
	movzbl	%r10b,%r11d
	cmp	$0,%r11d
	je	.L10
	movl	-49(%rbp), %r10d
	subl	$1,%r10d
	movl	%r10d, -49(%rbp)
	mov	%r11d, -53(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$10, %r11d
	cltd	
	idivl	%r11d
	movl	$65, %r11d
	addl	%edx,%r11d
	mov	%r11d, -57(%rbp)	# spill
	movl	-57(%rbp), %edi
	movl	$0, %eax
	call	_putchar
	addl	$1,%eax
	mov	%eax, -61(%rbp)	# spill
	movl	-61(%rbp), %edi
	movl	$0, %eax
	call	_putchar
	addl	$2,%eax
	mov	%eax, -65(%rbp)	# spill
	movl	-65(%rbp), %edi
	movl	$0, %eax
	call	_putchar
	addl	$3,%eax
	mov	%eax, -69(%rbp)	# spill
	movl	-69(%rbp), %edi
	movl	$0, %eax
	call	_putchar
	addl	$4,%eax
	mov	%eax, -73(%rbp)	# spill
	movl	-73(%rbp), %edi
	movl	$0, %eax
	call	_putchar
	mov	%eax, -77(%rbp)	# spill
	movl	$32, %edi
	movl	$0, %eax
	call	_putchar
	lea	_s(%rip),%r11
	movq	%r11, _s3(%rip)
	lea	_t(%rip),%r11
	movq	%r11, -45(%rbp)
	mov	%eax, -81(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$100, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, _i1(%rip)
	movl	$0, %eax
	call	_rand
	movl	$1000, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, %r11d
	movslq	%r11d,%r11
	movq	%r11, _l1(%rip)
	movl	$0, %eax
	call	_rand
	movl	$10, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, %r11d
	movb	%r11b, _c1(%rip)
	movl	$0, %eax
	call	_rand
	movl	$30, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, -4(%rbp)
	movl	$0, %eax
	call	_rand
	movl	$300, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, %r11d
	movslq	%r11d,%r11
	movq	%r11, -20(%rbp)
	movl	$0, %eax
	call	_rand
	movl	$3, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, %r11d
	movb	%r11b, -29(%rbp)
	lea	.L11,%r11
	mov	%r11, -89(%rbp)	# spill
	movl	-4(%rbp), %edx
	movl	_i1(%rip), %esi
	movq	-89(%rbp), %rdi
	movl	$0, %eax
	call	_printf
.L12:
	mov	%eax, -93(%rbp)	# spill
	movl	_i1(%rip), %eax
	movl	$3, %r11d
	cltd	
	idivl	%r11d
	movl	$0, %r11d
	cmpl	%r11d,%edx
	setne	%r11b
	movzbl	%r11b,%edx
	cmp	$0,%edx
	je	.L13
	lea	.L14,%r11
	movq	_p1(%rip), %r10
	mov	(%r10),%r10
	movl	-4(%rbp), %r9d
	imull	%r10d,%r9d
	movb	-29(%rbp), %r10b
	movsbl	%r10b,%r10d
	addl	%r10d,%r9d
	movl	%r9d, %r10d
	movslq	%r10d,%r10
	movq	_q1(%rip), %r9
	mov	(%r9),%r9
	movq	-37(%rbp), %r8
	mov	(%r8),%r8
	movb	%r8b, %cl
	movsbl	%cl,%ecx
	mov	%r11, -101(%rbp)	# spill
	mov	%r10, -109(%rbp)	# spill
	mov	%r9, -117(%rbp)	# spill
	mov	%ecx, -121(%rbp)	# spill
	mov	%edx, -125(%rbp)	# spill
	movl	-121(%rbp), %edi
	movl	$0, %eax
	call	_abs
	addl	$1,%eax
	movl	%eax, %r11d
	movslq	%r11d,%r11
	movq	-117(%rbp), %rax
	cqto	
	idivq	%r11
	movq	-109(%rbp), %r11
	subq	%rdx,%r11
	mov	%r11, -133(%rbp)	# spill
	movq	-133(%rbp), %rsi
	movq	-101(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	lea	.L15,%r11
	mov	%r11, -141(%rbp)	# spill
	mov	%eax, -145(%rbp)	# spill
	movl	-4(%rbp), %esi
	movl	_i1(%rip), %edi
	call	_f
	mov	%eax, -149(%rbp)	# spill
	movl	-149(%rbp), %esi
	movq	-141(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	mov	%eax, -153(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$20, %r11d
	cltd	
	idivl	%r11d
	movl	_i1(%rip), %r11d
	addl	%edx,%r11d
	movl	%r11d, _i1(%rip)
.L16:
	movq	-20(%rbp), %rdi
	movl	$0, %eax
	call	_labs
	movl	$5, %r11d
	movslq	%r11d,%r11
	cqto	
	idivq	%r11
	movl	$10, %r11d
	movslq	%r11d,%r11
	cmpl	%r11d,%eax
	setg	%r11b
	movzbl	%r11b,%eax
	cmp	$0,%eax
	je	.L17
	mov	%eax, -157(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$7, %r11d
	cltd	
	idivl	%r11d
	addl	$1,%edx
	movl	%edx, %r11d
	movslq	%r11d,%r11
	movq	-20(%rbp), %rax
	cqto	
	idivq	%r11
	movq	%rax, -20(%rbp)
	movl	$0, %r11d
	movslq	%r11d,%r11
	movq	-20(%rbp), %r10
	cmpl	%r11d,%r10d
	setne	%r11b
	movzbl	%r11b,%r10d
	cmp	$0,%r10d
	je	.L19
	lea	.L20,%r11
	movb	_c1(%rip), %r9b
	movsbq	%r9b,%r9
	movb	-29(%rbp), %r8b
	movsbq	%r8b,%r8
	movq	-37(%rbp), %rcx
	mov	(%rcx),%rcx
	movb	%cl, %dl
	movsbq	%dl,%rdx
	movq	%rdx, %rax
	movq	-20(%rbp), %rcx
	cqto	
	idivq	%rcx
	imulq	%rax,%r8
	addq	%r8,%r9
	movq	-28(%rbp), %r8
	mov	(%r8),%r8
	addq	%r8,%r9
	movq	%r9, %r8
	movb	%r8b, %r9b
	movsbl	%r9b,%r9d
	mov	%r11, -165(%rbp)	# spill
	mov	%r10d, -169(%rbp)	# spill
	mov	%r9d, -173(%rbp)	# spill
	movl	-173(%rbp), %esi
	movq	-165(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	jmp	.L19
.L19:
	movb	_c1(%rip), %r11b
	movsbl	%r11b,%r11d
	addl	$2,%r11d
	movl	%r11d, %r10d
	movb	%r10b, _c1(%rip)
	jmp	.L16
.L17:
	movl	-4(%rbp), %r11d
	imull	$3,%r11d
	movl	%r11d, -4(%rbp)
	jmp	.L12
.L13:
.L21:
	movb	_c1(%rip), %r11b
	movsbl	%r11b,%r11d
	movl	$1, %r10d
	cmpl	%r10d,%r11d
	setg	%r10b
	movzbl	%r10b,%r11d
	cmp	$0,%r11d
	je	.L22
	movb	_c1(%rip), %r10b
	movsbl	%r10b,%r10d
	mov	%r11d, -177(%rbp)	# spill
	mov	%r10d, -181(%rbp)	# spill
	mov	%eax, -185(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$3, %r11d
	cltd	
	idivl	%r11d
	movl	-181(%rbp), %r11d
	subl	%edx,%r11d
	movl	%r11d, %r10d
	movb	%r10b, _c1(%rip)
	lea	.L23,%r11
	movb	-29(%rbp), %r10b
	movsbq	%r10b,%r10
	movq	_l1(%rip), %r9
	cmpl	%r10d,%r9d
	setg	%r10b
	movzbl	%r10b,%r9d
	cmp	$0,%r9d
	jne	.L24
	movb	-29(%rbp), %r10b
	movsbl	%r10b,%r10d
	movq	_p1(%rip), %r8
	mov	(%r8),%r8
	cmpl	%r8d,%r10d
	setle	%r8b
	movzbl	%r8b,%r10d
	cmp	$0,%r10d
	je	.L27
	movq	_p1(%rip), %r8
	movq	-12(%rbp), %rcx
	cmpl	%ecx,%r8d
	sete	%cl
	movzbl	%cl,%r8d
	cmp	$0,%r8d
	je	.L26
	mov	$1,%r10d
	jmp	.L27
.L26:
	mov	$0,%r10d
.L27:
	cmp	$0,%r10d
	jne	.L24
	mov	$0,%r9d
	jmp	.L25
.L24:
	mov	$1,%r9d
.L25:
	mov	%r11, -193(%rbp)	# spill
	mov	%r9d, -197(%rbp)	# spill
	mov	%r8d, -201(%rbp)	# spill
	movl	-197(%rbp), %esi
	movq	-193(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	movq	-28(%rbp), %r11
	mov	(%r11),%r11
	movq	-20(%rbp), %r10
	imulq	%r11,%r10
	movq	-12(%rbp), %r11
	mov	(%r11),%r11
	addl	$1,%r11d
	movl	%r11d, %r9d
	movslq	%r9d,%r9
	mov	%eax, -205(%rbp)	# spill
	movq	%r10, %rax
	cqto	
	idivq	%r9
	movl	$5, %r11d
	movslq	%r11d,%r11
	cmpl	%r11d,%edx
	setg	%r11b
	movzbl	%r11b,%edx
	cmp	$0,%edx
	je	.L28
	lea	.L30,%r11
	movb	-29(%rbp), %r10b
	movsbl	%r10b,%r10d
	movl	_i1(%rip), %r9d
	imull	%r10d,%r9d
	movl	%r9d, %r10d
	movslq	%r10d,%r10
	movq	_l1(%rip), %r9
	subq	%r10,%r9
	movq	_s1(%rip), %r10
	mov	(%r10),%r10
	movb	%r10b, %r8b
	movsbl	%r8b,%r8d
	movq	-12(%rbp), %r10
	mov	(%r10),%r10
	movl	%r8d, %eax
	cltd	
	idivl	%r10d
	movl	%edx, %r10d
	movslq	%r10d,%r10
	subq	%r10,%r9
	movq	_l1(%rip), %r10
	movq	-20(%rbp), %r8
	cmpl	%r8d,%r10d
	setge	%r8b
	movzbl	%r8b,%r10d
	movl	%r10d, %r8d
	movslq	%r8d,%r8
	addq	%r8,%r9
	mov	%r11, -213(%rbp)	# spill
	mov	%r9, -221(%rbp)	# spill
	movq	-221(%rbp), %rsi
	movq	-213(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	jmp	.L29
.L28:
	mov	%eax, -225(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movq	-37(%rbp), %r11
	mov	(%r11),%r11
	movb	%r11b, %r10b
	movsbl	%r10b,%r10d
	mov	%r10d, -229(%rbp)	# spill
	mov	%eax, -233(%rbp)	# spill
	movl	-229(%rbp), %edi
	movl	$0, %eax
	call	_abs
	addl	$4,%eax
	mov	%eax, -237(%rbp)	# spill
	movl	-233(%rbp), %eax
	movl	-237(%rbp), %r11d
	cltd	
	idivl	%r11d
	movl	$10, %r11d
	cmpl	%r11d,%edx
	setl	%r11b
	movzbl	%r11b,%edx
	cmp	$0,%edx
	je	.L31
	lea	.L33,%r11
	movq	_s1(%rip), %r10
	mov	(%r10),%r10
	movb	%r10b, %r9b
	movsbl	%r9b,%r9d
	lea	-37(%rbp),%r10
	mov	(%r10),%r10
	mov	(%r10),%r10
	movb	%r10b, %r8b
	movsbl	%r8b,%r8d
	addl	%r8d,%r9d
	movb	_c1(%rip), %r10b
	movsbq	%r10b,%r10
	movq	-20(%rbp), %r8
	cmpl	%r8d,%r10d
	setne	%r8b
	movzbl	%r8b,%r10d
	movb	-29(%rbp), %r8b
	movsbl	%r8b,%r8d
	movl	-4(%rbp), %ecx
	cmpl	%ecx,%r8d
	setne	%cl
	movzbl	%cl,%r8d
	imull	%r8d,%r10d
	addl	%r10d,%r9d
	mov	%r11, -245(%rbp)	# spill
	mov	%r9d, -249(%rbp)	# spill
	mov	%edx, -253(%rbp)	# spill
	movl	-249(%rbp), %esi
	movq	-245(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	jmp	.L32
.L31:
	lea	.L34,%r11
	movl	$7, %r10d
	movslq	%r10d,%r10
	mov	%eax, -257(%rbp)	# spill
	movq	_l1(%rip), %rax
	cqto	
	idivq	%r10
	movb	-29(%rbp), %r10b
	movsbq	%r10b,%r10
	subq	%r10,%rax
	movq	_s1(%rip), %r10
	mov	(%r10),%r10
	movb	%r10b, %r9b
	movsbl	%r9b,%r9d
	movq	-12(%rbp), %r10
	mov	(%r10),%r10
	mov	%rax, -265(%rbp)	# spill
	movl	%r9d, %eax
	cltd	
	idivl	%r10d
	movl	%edx, %r10d
	movslq	%r10d,%r10
	movq	-265(%rbp), %r9
	subq	%r10,%r9
	movq	_l1(%rip), %r10
	movq	-20(%rbp), %r8
	cmpl	%r8d,%r10d
	setge	%r8b
	movzbl	%r8b,%r10d
	movl	%r10d, %r8d
	movslq	%r8d,%r8
	addq	%r8,%r9
	mov	%r11, -273(%rbp)	# spill
	mov	%r9, -281(%rbp)	# spill
	movq	-281(%rbp), %rsi
	movq	-273(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	lea	_c1(%rip),%r11
	movq	%r11, _s1(%rip)
	lea	-29(%rbp),%r11
	movq	%r11, -37(%rbp)
.L32:
.L29:
	movl	_i1(%rip), %r11d
	movslq	%r11d,%r11
	imulq	$1,%r11
	movq	_s1(%rip), %r10
	mov	(%r10),%r10
	movb	%r10b, %r9b
	movsbq	%r9b,%r9
	imulq	$1,%r9
	addq	-37(%rbp),%r9
	addq	%r9,%r11
	movb	_c1(%rip), %r10b
	movsbq	%r10b,%r10
	movq	-28(%rbp), %r9
	mov	(%r9),%r9
	imulq	%r9,%r10
	imulq	$1,%r10
	addq	%r10,%r11
	cmp	$0,%r11
	je	.L36
	lea	.L37,%r10
	movq	_l1(%rip), %r9
	addq	-20(%rbp),%r9
	movb	_c1(%rip), %r8b
	movsbq	%r8b,%r8
	addq	%r8,%r9
	mov	%r11, -289(%rbp)	# spill
	mov	%r10, -297(%rbp)	# spill
	mov	%r9, -305(%rbp)	# spill
	mov	%eax, -309(%rbp)	# spill
	movq	-305(%rbp), %rsi
	movq	-297(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	jmp	.L36
.L36:
	movq	$100, %r11
	imulq	$1,%r11
	lea	_t(%rip),%r10
	addq	%r10,%r11
	movl	$1, %r10d
	movslq	%r10d,%r10
	imulq	$1,%r10
	subq	%r10,%r11
	movq	-45(%rbp), %r10
	cmpl	%r10d,%r11d
	setge	%r10b
	movzbl	%r10b,%r11d
	cmp	$0,%r11d
	je	.L39
	mov	%r11d, -313(%rbp)	# spill
	mov	%eax, -317(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$26, %r11d
	cltd	
	idivl	%r11d
	movl	$65, %r11d
	addl	%edx,%r11d
	movl	%r11d, %r10d
	movq	-45(%rbp), %r11
	movb	%r10b, (%r11)
	movl	$1, %r9d
	movslq	%r9d,%r9
	imulq	$1,%r9
	movq	-45(%rbp), %r8
	addq	%r9,%r8
	movq	%r8, -45(%rbp)
	jmp	.L39
.L39:
	mov	%r11, -325(%rbp)	# spill
	mov	%r10b, -326(%rbp)	# spill
	movl	$0, %eax
	call	_rand
	movl	$100, %r11d
	cltd	
	idivl	%r11d
	movl	%edx, %r11d
	movslq	%r11d,%r11
	movq	_l1(%rip), %r10
	subq	%r11,%r10
	movq	%r10, _l1(%rip)
	jmp	.L21
.L22:
	lea	_t(%rip),%r11
	movq	%r11, %r10
	movq	%r10, -12(%rbp)
	movl	$0, %r11d
	movl	%r11d, _i1(%rip)
	movl	$0, %r11d
	movq	_s3(%rip), %r10
	movb	%r11b, (%r10)
	movl	$0, %r9d
	movq	-45(%rbp), %r8
	movb	%r9b, (%r8)
.L40:
	movl	_i1(%rip), %ecx
	movl	$5, %edx
	cmpl	%edx,%ecx
	setl	%dl
	movzbl	%dl,%ecx
	cmp	$0,%ecx
	je	.L41
	lea	.L42,%rdx
	movl	_i1(%rip), %esi
	movslq	%esi,%rsi
	imulq	$4,%rsi
	movq	-12(%rbp), %rdi
	addq	%rsi,%rdi
	mov	(%rdi),%rdi
	mov	%r11b, -327(%rbp)	# spill
	mov	%r10, -335(%rbp)	# spill
	mov	%r9b, -336(%rbp)	# spill
	mov	%r8, -344(%rbp)	# spill
	mov	%ecx, -348(%rbp)	# spill
	mov	%rdx, -356(%rbp)	# spill
	mov	%edi, -360(%rbp)	# spill
	movl	-360(%rbp), %esi
	movq	-356(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	movl	_i1(%rip), %r11d
	addl	$1,%r11d
	movl	%r11d, _i1(%rip)
	jmp	.L40
.L41:
	lea	.L43,%r11
	movq	_s1(%rip), %r10
	mov	(%r10),%r10
	movb	%r10b, %r9b
	movsbl	%r9b,%r9d
	movq	-37(%rbp), %r10
	mov	(%r10),%r10
	movb	%r10b, %r8b
	movsbl	%r8b,%r8d
	movq	_p1(%rip), %r10
	mov	(%r10),%r10
	imull	%r10d,%r8d
	movq	-12(%rbp), %r10
	mov	(%r10),%r10
	imull	%r10d,%r8d
	addl	%r8d,%r9d
	mov	%r11, -368(%rbp)	# spill
	mov	%r9d, -372(%rbp)	# spill
	mov	%eax, -376(%rbp)	# spill
	movl	-372(%rbp), %esi
	movq	-368(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	lea	.L44,%r11
	lea	_s(%rip),%r10
	lea	_t(%rip),%r9
	mov	%r11, -384(%rbp)	# spill
	mov	%r10, -392(%rbp)	# spill
	mov	%r9, -400(%rbp)	# spill
	mov	%eax, -404(%rbp)	# spill
	movq	-400(%rbp), %rdx
	movq	-392(%rbp), %rsi
	movq	-384(%rbp), %rdi
	movl	$0, %eax
	call	_printf
	mov	%eax, -408(%rbp)	# spill
	movl	$10, %edi
	movl	$0, %eax
	call	_putchar
	jmp	.L9
.L10:
.L8:
	movq	%rbp, %rsp
	popq	%rbp
	ret
	.set	main.size, 416
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
.L11:	.asciz	"i1 = %d  i2 = %d\n"
.L14:	.asciz	"%d "
.L15:	.asciz	"%d "
.L20:	.asciz	"%d "
.L23:	.asciz	"%d "
.L30:	.asciz	"%ld "
.L33:	.asciz	"%d "
.L34:	.asciz	"%d "
.L37:	.asciz	"%d "
.L42:	.asciz	"%d "
.L43:	.asciz	"%d "
.L44:	.asciz	"%.20s %.20s"
