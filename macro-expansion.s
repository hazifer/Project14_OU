	.file	"macro-expansion.c"
	.text
	.section	.rodata
.LC0:
	.string	"r"
	.text
	.globl	handle_filename_extension
	.type	handle_filename_extension, @function
handle_filename_extension:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	-32(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	testq	%rax, %rax
	je	.L2
	movq	-24(%rbp), %rax
	movq	%rax, -16(%rbp)
	jmp	.L3
.L2:
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	addq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L4
	movq	-40(%rbp), %rax
	movl	$2, (%rax)
	movq	-40(%rbp), %rax
	addq	$8, %rax
	movabsq	$7575180396224210185, %rsi
	movabsq	$2337212502790447214, %rdi
	movq	%rsi, (%rax)
	movq	%rdi, 8(%rax)
	movabsq	$8028074728682977381, %rsi
	movabsq	$7594793497474244718, %rdi
	movq	%rsi, 16(%rax)
	movq	%rdi, 24(%rax)
	movl	$2123116, 32(%rax)
	movq	-40(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	strcat@PLT
	movq	-40(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	strcat@PLT
	movq	-40(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %rsi
	movl	$0, %eax
	movq	%rsi, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movabsq	$7954602093478879290, %rsi
	movabsq	$7165064744911533095, %rdi
	movq	%rsi, (%rax)
	movq	%rdi, 8(%rax)
	movabsq	$8462103605466723425, %rsi
	movabsq	$8245929725128632423, %rdi
	movq	%rsi, 16(%rax)
	movq	%rdi, 24(%rax)
	movabsq	$7526676553110265977, %rdx
	movabsq	$7021788497383006309, %rcx
	movq	%rdx, 32(%rax)
	movq	%rcx, 40(%rax)
	movl	$667245, 48(%rax)
	movl	$0, %eax
	jmp	.L5
.L4:
	movq	-16(%rbp), %rax
	movb	$0, (%rax)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcat@PLT
	movq	-32(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcat@PLT
.L3:
	movq	-16(%rbp), %rax
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L6
	movl	$0, %eax
	jmp	.L5
.L6:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	movq	-40(%rbp), %rax
	movl	$1, (%rax)
	movq	-40(%rbp), %rax
	addq	$8, %rax
	movabsq	$8319104414228247305, %rsi
	movabsq	$7885630463676282214, %rdi
	movq	%rsi, (%rax)
	movq	%rdi, 8(%rax)
	movabsq	$7863395336296164464, %rdx
	movabsq	$8104338719436333921, %rcx
	movq	%rdx, 16(%rax)
	movq	%rcx, 24(%rax)
	movabsq	$7359003215912988257, %rsi
	movabsq	$2334391181802238575, %rdi
	movq	%rsi, 32(%rax)
	movq	%rdi, 40(%rax)
	movb	$0, 48(%rax)
	movq	-40(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	strcat@PLT
	movq	-40(%rbp), %rax
	leaq	8(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %rsi
	movl	$0, %eax
	movq	%rsi, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	movq	-16(%rbp), %rax
.L5:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	handle_filename_extension, .-handle_filename_extension
	.section	.rodata
.LC1:
	.string	".as"
.LC2:
	.string	"w"
	.text
	.globl	expand_macros
	.type	expand_macros, @function
expand_macros:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	$0, -48(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -40(%rbp)
	cmpq	$0, -40(%rbp)
	jne	.L8
	movq	-56(%rbp), %rsi
	movq	-64(%rbp), %rax
	movl	$0, %r8d
	movl	$4, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	log_error@PLT
	movl	$1, %eax
	jmp	.L9
.L8:
	movq	-56(%rbp), %rdx
	movq	-40(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcpy@PLT
	movq	-40(%rbp), %rax
	leaq	.LC1(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	addq	$2, %rax
	movb	$109, (%rax)
	movq	-56(%rbp), %rax
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L10
	movq	-56(%rbp), %rsi
	movq	-64(%rbp), %rax
	movl	$0, %r8d
	movl	$3, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	log_error@PLT
	movl	$1, %eax
	jmp	.L9
.L10:
	movq	-40(%rbp), %rax
	leaq	.LC2(%rip), %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L11
	movq	-40(%rbp), %rsi
	movq	-64(%rbp), %rax
	movl	$0, %r8d
	movl	$5, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	log_error@PLT
	movl	$1, %eax
	jmp	.L9
.L11:
	movl	$81, %edi
	call	malloc@PLT
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L12
	movq	-8(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-64(%rbp), %rax
	movl	$0, %r8d
	movl	$6, %ecx
	movq	%rax, %rdi
	call	log_error@PLT
	movl	$1, %eax
	jmp	.L9
.L12:
	movq	-64(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	allocate_macro_array_memory
	movq	%rax, -48(%rbp)
	cmpq	$0, -48(%rbp)
	jne	.L13
	movq	-64(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$16, %eax
	jne	.L14
	movq	-8(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-64(%rbp), %rax
	movl	$0, %r8d
	movl	$16, %ecx
	movq	%rax, %rdi
	call	log_error@PLT
	movl	$1, %eax
	jmp	.L9
.L14:
	movq	-8(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-64(%rbp), %rax
	movl	$0, %r8d
	movl	$6, %ecx
	movq	%rax, %rdi
	call	log_error@PLT
	movl	$1, %eax
	jmp	.L9
.L13:
	movq	-64(%rbp), %r8
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	-40(%rbp), %rsi
	movq	-56(%rbp), %rax
	subq	$8, %rsp
	pushq	-48(%rbp)
	movq	%r8, %r9
	movq	%rdi, %r8
	movq	%rax, %rdi
	call	expand_macros_memory_allocated
	addq	$16, %rsp
	movq	-64(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	je	.L15
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	remove@PLT
.L15:
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	movl	$0, %eax
.L9:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	expand_macros, .-expand_macros
	.globl	expand_macros_memory_allocated
	.type	expand_macros_memory_allocated, @function
expand_macros_memory_allocated:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movq	%r8, -88(%rbp)
	movq	%r9, -96(%rbp)
	movl	$17, -36(%rbp)
	movl	$0, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, -28(%rbp)
	jmp	.L17
.L33:
	movl	$0, -32(%rbp)
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$59, %al
	je	.L18
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$10, %al
	jne	.L19
.L18:
	movq	-80(%rbp), %rax
	movq	%rax, %rsi
	movl	$10, %edi
	call	fputc@PLT
	jmp	.L17
.L19:
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$58, %al
	jne	.L20
	addq	$1, -16(%rbp)
	movl	$1, -32(%rbp)
.L20:
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	skip_blanks@PLT
	movq	%rax, -16(%rbp)
	cmpl	$17, -36(%rbp)
	jne	.L21
	movl	-28(%rbp), %r9d
	movq	-16(%rbp), %r8
	movq	-80(%rbp), %rcx
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rsi
	movq	-56(%rbp), %rax
	subq	$8, %rsp
	movl	-24(%rbp), %edi
	pushq	%rdi
	pushq	16(%rbp)
	pushq	-96(%rbp)
	movq	%rax, %rdi
	call	expand_macros_handle_command_state
	addq	$32, %rsp
	movl	%eax, -20(%rbp)
	cmpl	$0, -32(%rbp)
	je	.L22
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	is_newline_needed
	testb	%al, %al
	je	.L22
	movq	-80(%rbp), %rax
	movq	%rax, %rsi
	movl	$10, %edi
	call	fputc@PLT
.L22:
	cmpl	$0, -20(%rbp)
	je	.L23
	cmpl	$21, -20(%rbp)
	jne	.L24
.L23:
	movl	$17, -36(%rbp)
	jmp	.L17
.L24:
	cmpl	$19, -20(%rbp)
	je	.L25
	movl	-28(%rbp), %edi
	movl	-20(%rbp), %ecx
	movq	-88(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-96(%rbp), %rax
	movl	%edi, %r8d
	movq	%rax, %rdi
	call	log_error@PLT
	jmp	.L26
.L25:
	movl	$19, -36(%rbp)
	jmp	.L17
.L21:
	cmpl	$19, -36(%rbp)
	jne	.L17
	movl	-28(%rbp), %r9d
	movq	-16(%rbp), %r8
	movq	-80(%rbp), %rcx
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rsi
	movq	-56(%rbp), %rax
	subq	$8, %rsp
	movl	-24(%rbp), %edi
	pushq	%rdi
	pushq	16(%rbp)
	pushq	-96(%rbp)
	movq	%rax, %rdi
	call	expand_macros_handle_collect_macro_content_state
	addq	$32, %rsp
	movl	%eax, -20(%rbp)
	cmpl	$20, -20(%rbp)
	jne	.L27
	jmp	.L17
.L27:
	cmpl	$8, -20(%rbp)
	je	.L28
	cmpl	$12, -20(%rbp)
	jne	.L29
.L28:
	movl	-28(%rbp), %edi
	movl	-20(%rbp), %ecx
	movq	-88(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-96(%rbp), %rax
	movl	%edi, %r8d
	movq	%rax, %rdi
	call	log_error@PLT
	jmp	.L26
.L29:
	addl	$1, -24(%rbp)
	movq	-96(%rbp), %rdx
	movl	-24(%rbp), %eax
	movl	%eax, %esi
	movq	16(%rbp), %rdi
	call	increment_macro_array_index
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L30
	movq	-96(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$6, %eax
	jne	.L30
	movq	-96(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$16, %eax
	jne	.L30
	movq	-96(%rbp), %rax
	movl	(%rax), %ecx
	movl	-28(%rbp), %edi
	movq	-88(%rbp), %rdx
	movq	-56(%rbp), %rsi
	movq	-96(%rbp), %rax
	movl	%edi, %r8d
	movq	%rax, %rdi
	call	log_error@PLT
	jmp	.L26
.L30:
	cmpq	$0, -8(%rbp)
	je	.L31
	movq	-8(%rbp), %rax
	jmp	.L32
.L31:
	movq	16(%rbp), %rax
.L32:
	movq	%rax, 16(%rbp)
	movl	$17, -36(%rbp)
.L17:
	movq	-72(%rbp), %rdx
	movq	-88(%rbp), %rax
	movl	$81, %esi
	movq	%rax, %rdi
	call	fgets@PLT
	testq	%rax, %rax
	je	.L26
	addl	$1, -28(%rbp)
	movl	-28(%rbp), %r8d
	movq	-88(%rbp), %rdi
	movq	-80(%rbp), %rcx
	movq	-72(%rbp), %rdx
	movq	-64(%rbp), %rsi
	movq	-56(%rbp), %rax
	subq	$8, %rsp
	pushq	-96(%rbp)
	movl	%r8d, %r9d
	movq	%rdi, %r8
	movq	%rax, %rdi
	call	expand_macros_handle_label
	addq	$16, %rsp
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L33
.L26:
	movq	-96(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	je	.L34
	movl	$1, %eax
	jmp	.L35
.L34:
	movl	$0, %eax
.L35:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	expand_macros_memory_allocated, .-expand_macros_memory_allocated
	.globl	expand_macros_handle_label
	.type	expand_macros_handle_label, @function
expand_macros_handle_label:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	%rcx, -48(%rbp)
	movq	%r8, -56(%rbp)
	movl	%r9d, -60(%rbp)
	movq	-56(%rbp), %rax
	movl	$58, %esi
	movq	%rax, %rdi
	call	strchr@PLT
	movq	%rax, -8(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	skip_blanks@PLT
	movq	%rax, -56(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L37
	movq	-56(%rbp), %rax
	jmp	.L38
.L37:
	movq	-8(%rbp), %rax
	movzbl	1(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movb	$0, (%rax)
	movq	-48(%rbp), %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	fputs@PLT
	movq	-8(%rbp), %rax
	leaq	1(%rax), %rdx
	movzbl	-9(%rbp), %eax
	movb	%al, (%rdx)
	movq	-8(%rbp), %rax
.L38:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	expand_macros_handle_label, .-expand_macros_handle_label
	.globl	expand_macros_handle_command_state
	.type	expand_macros_handle_command_state, @function
expand_macros_handle_command_state:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	addq	$-128, %rsp
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%rdx, -88(%rbp)
	movq	%rcx, -96(%rbp)
	movq	%r8, -104(%rbp)
	movl	%r9d, -108(%rbp)
	movq	16(%rbp), %rax
	movq	%rax, -120(%rbp)
	movq	24(%rbp), %rax
	movq	%rax, -128(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-120(%rbp), %rdx
	movq	-104(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	read_till_macr_keyword
	movq	%rax, -56(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L40
	movq	-120(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$7, %eax
	jne	.L40
	movl	$7, %eax
	jmp	.L50
.L40:
	cmpq	$0, -56(%rbp)
	jne	.L42
	movq	-104(%rbp), %rax
	movq	%rax, %rdi
	call	skip_blanks@PLT
	movq	%rax, -56(%rbp)
	leaq	-48(%rbp), %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	read_word@PLT
	movq	-128(%rbp), %rdx
	leaq	-48(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	get_macro_name_index
	movsbl	%al, %eax
	movl	%eax, -60(%rbp)
	cmpl	$0, -60(%rbp)
	js	.L43
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	addq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	read_word@PLT
	testl	%eax, %eax
	je	.L44
	movl	$13, %eax
	jmp	.L50
.L44:
	movl	-60(%rbp), %edx
	movq	-128(%rbp), %rcx
	movq	-96(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	expand_macro
	movl	$21, %eax
	jmp	.L50
.L43:
	movq	-96(%rbp), %rdx
	movq	-104(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	fputs@PLT
	movl	$0, %eax
	jmp	.L50
.L42:
	addq	$4, -56(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, %rdi
	call	skip_blanks@PLT
	movq	%rax, -56(%rbp)
	leaq	-48(%rbp), %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	read_word@PLT
	testl	%eax, %eax
	jne	.L45
	movl	$9, %eax
	jmp	.L50
.L45:
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	verify_not_reserved
	testb	%al, %al
	jne	.L46
	movl	$10, %eax
	jmp	.L50
.L46:
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	verify_macro_name_syntax
	testb	%al, %al
	jne	.L47
	movl	$14, %eax
	jmp	.L50
.L47:
	movq	-128(%rbp), %rdx
	leaq	-48(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	get_macro_name_index
	testb	%al, %al
	js	.L48
	movl	$15, %eax
	jmp	.L50
.L48:
	leaq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	strlen@PLT
	addq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	read_word@PLT
	testl	%eax, %eax
	je	.L49
	movl	$11, %eax
	jmp	.L50
.L49:
	movl	32(%rbp), %eax
	cltq
	imulq	$4082, %rax, %rdx
	movq	-128(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	strcpy@PLT
	movl	$19, %eax
.L50:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L51
	call	__stack_chk_fail@PLT
.L51:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	expand_macros_handle_command_state, .-expand_macros_handle_command_state
	.globl	expand_macros_handle_collect_macro_content_state
	.type	expand_macros_handle_collect_macro_content_state, @function
expand_macros_handle_collect_macro_content_state:
.LFB5:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	%rcx, -48(%rbp)
	movq	%r8, -56(%rbp)
	movl	%r9d, -60(%rbp)
	movq	-56(%rbp), %rax
	movq	16(%rbp), %rsi
	movq	%rax, %rdi
	call	read_till_endmacr_keyword
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L53
	movq	16(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$8, %eax
	jne	.L53
	movl	$8, %eax
	jmp	.L54
.L53:
	cmpq	$0, -8(%rbp)
	jne	.L55
	movl	32(%rbp), %eax
	cltq
	imulq	$4082, %rax, %rdx
	movq	24(%rbp), %rax
	addq	%rdx, %rax
	leaq	32(%rax), %rdx
	movq	-56(%rbp), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	strcat@PLT
	movl	$20, %eax
	jmp	.L54
.L55:
	addq	$7, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	read_word@PLT
	testl	%eax, %eax
	je	.L56
	movl	$12, %eax
	jmp	.L54
.L56:
	movl	$17, %eax
.L54:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	expand_macros_handle_collect_macro_content_state, .-expand_macros_handle_collect_macro_content_state
	.section	.rodata
.LC3:
	.string	"macr "
.LC4:
	.string	"macr\t"
.LC5:
	.string	"macr\n"
.LC6:
	.string	" macr "
.LC7:
	.string	"\tmacr "
.LC8:
	.string	" macr\t"
.LC9:
	.string	"\tmacr\t"
.LC10:
	.string	" macr\n"
.LC11:
	.string	"\tmacr\n"
	.text
	.globl	read_till_macr_keyword
	.type	read_till_macr_keyword, @function
read_till_macr_keyword:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-72(%rbp), %rax
	leaq	.LC3(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L58
	movq	-72(%rbp), %rax
	leaq	.LC4(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L58:
	cmpq	$0, -56(%rbp)
	jne	.L59
	movq	-72(%rbp), %rax
	leaq	.LC5(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L59:
	movq	-72(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jne	.L60
	movq	-56(%rbp), %rax
	jmp	.L69
.L60:
	leaq	-48(%rbp), %rdx
	movq	-72(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	read_word@PLT
	movl	%eax, -60(%rbp)
	movl	-60(%rbp), %eax
	cltq
	addq	%rax, -72(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L62
	movq	-72(%rbp), %rax
	leaq	.LC6(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L62:
	cmpq	$0, -56(%rbp)
	jne	.L63
	movq	-72(%rbp), %rax
	leaq	.LC7(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L63:
	cmpq	$0, -56(%rbp)
	jne	.L64
	movq	-72(%rbp), %rax
	leaq	.LC8(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L64:
	cmpq	$0, -56(%rbp)
	jne	.L65
	movq	-72(%rbp), %rax
	leaq	.LC9(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L65:
	cmpq	$0, -56(%rbp)
	jne	.L66
	movq	-72(%rbp), %rax
	leaq	.LC10(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L66:
	cmpq	$0, -56(%rbp)
	jne	.L67
	movq	-72(%rbp), %rax
	leaq	.LC11(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L67:
	cmpq	$0, -56(%rbp)
	jne	.L68
	movl	$0, %eax
	jmp	.L69
.L68:
	movq	-80(%rbp), %rax
	movl	$7, (%rax)
	movl	$0, %eax
.L69:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L70
	call	__stack_chk_fail@PLT
.L70:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	read_till_macr_keyword, .-read_till_macr_keyword
	.section	.rodata
.LC12:
	.string	"endmacr "
.LC13:
	.string	"endmacr\t"
.LC14:
	.string	"endmacr\n"
.LC15:
	.string	" endmacr "
.LC16:
	.string	"\tendmacr "
.LC17:
	.string	" endmacr\t"
.LC18:
	.string	"\tendmacr\t"
.LC19:
	.string	" endmacr\n"
.LC20:
	.string	"\tendmacr\n"
	.text
	.globl	read_till_endmacr_keyword
	.type	read_till_endmacr_keyword, @function
read_till_endmacr_keyword:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movq	-72(%rbp), %rax
	leaq	.LC12(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L72
	movq	-72(%rbp), %rax
	leaq	.LC13(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L72:
	cmpq	$0, -56(%rbp)
	jne	.L73
	movq	-72(%rbp), %rax
	leaq	.LC14(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L73:
	movq	-72(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jne	.L74
	movq	-56(%rbp), %rax
	jmp	.L83
.L74:
	leaq	-48(%rbp), %rdx
	movq	-72(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	read_word@PLT
	movl	%eax, -60(%rbp)
	movl	-60(%rbp), %eax
	cltq
	addq	%rax, -72(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L76
	movq	-72(%rbp), %rax
	leaq	.LC15(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L76:
	cmpq	$0, -56(%rbp)
	jne	.L77
	movq	-72(%rbp), %rax
	leaq	.LC16(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L77:
	cmpq	$0, -56(%rbp)
	jne	.L78
	movq	-72(%rbp), %rax
	leaq	.LC17(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L78:
	cmpq	$0, -56(%rbp)
	jne	.L79
	movq	-72(%rbp), %rax
	leaq	.LC18(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L79:
	cmpq	$0, -56(%rbp)
	jne	.L80
	movq	-72(%rbp), %rax
	leaq	.LC19(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L80:
	cmpq	$0, -56(%rbp)
	jne	.L81
	movq	-72(%rbp), %rax
	leaq	.LC20(%rip), %rsi
	movq	%rax, %rdi
	call	strstr@PLT
	movq	%rax, -56(%rbp)
.L81:
	cmpq	$0, -56(%rbp)
	jne	.L82
	movl	$0, %eax
	jmp	.L83
.L82:
	movq	-80(%rbp), %rax
	movl	$8, (%rax)
	movl	$0, %eax
.L83:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L84
	call	__stack_chk_fail@PLT
.L84:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	read_till_endmacr_keyword, .-read_till_endmacr_keyword
	.section	.rodata
.LC21:
	.string	"big"
.LC22:
	.string	"nob"
.LC23:
	.string	"test9"
.LC24:
	.string	"haha"
	.text
	.globl	verify_not_reserved
	.type	verify_not_reserved, @function
verify_not_reserved:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movq	%rdi, -72(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC21(%rip), %rax
	movq	%rax, -48(%rbp)
	leaq	.LC22(%rip), %rax
	movq	%rax, -40(%rbp)
	leaq	.LC23(%rip), %rax
	movq	%rax, -32(%rbp)
	leaq	.LC24(%rip), %rax
	movq	%rax, -24(%rbp)
	movl	$4, -52(%rbp)
	subl	$1, -52(%rbp)
	jmp	.L86
.L89:
	movl	-52(%rbp), %eax
	cltq
	movq	-48(%rbp,%rax,8), %rax
	movq	-72(%rbp), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L87
	movl	$0, %eax
	jmp	.L90
.L87:
	subl	$1, -52(%rbp)
.L86:
	cmpl	$0, -52(%rbp)
	jns	.L89
	movl	$1, %eax
.L90:
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L91
	call	__stack_chk_fail@PLT
.L91:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	verify_not_reserved, .-verify_not_reserved
	.globl	verify_macro_name_syntax
	.type	verify_macro_name_syntax, @function
verify_macro_name_syntax:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	call	__ctype_b_loc@PLT
	movq	(%rax), %rdx
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$2048, %eax
	testl	%eax, %eax
	je	.L95
	movl	$0, %eax
	jmp	.L94
.L96:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rcx
	movq	-8(%rbp), %rax
	leaq	1(%rax), %rdx
	movq	%rdx, -8(%rbp)
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	.L95
	movl	$0, %eax
	jmp	.L94
.L95:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L96
	movl	$1, %eax
.L94:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	verify_macro_name_syntax, .-verify_macro_name_syntax
	.globl	get_macro_name_index
	.type	get_macro_name_index, @function
get_macro_name_index:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L98
.L101:
	movq	-32(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L99
	movl	-4(%rbp), %eax
	jmp	.L100
.L99:
	addq	$4082, -32(%rbp)
	addl	$1, -4(%rbp)
.L98:
	movq	-32(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L101
	movl	$-1, %eax
.L100:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	get_macro_name_index, .-get_macro_name_index
	.globl	allocate_macro_array_memory
	.type	allocate_macro_array_memory, @function
allocate_macro_array_memory:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movzbl	macro_multiplier_factor.2176(%rip), %eax
	addl	$1, %eax
	movb	%al, macro_multiplier_factor.2176(%rip)
	movzbl	macro_multiplier_factor.2176(%rip), %eax
	movsbl	%al, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	cltq
	movq	%rax, -8(%rbp)
	movzbl	macro_multiplier_factor.2176(%rip), %eax
	cmpb	$10, %al
	jle	.L103
	movq	-32(%rbp), %rax
	movl	$16, (%rax)
	movl	$0, %eax
	jmp	.L104
.L103:
	cmpq	$0, -24(%rbp)
	je	.L105
	movq	-8(%rbp), %rax
	imulq	$4082, %rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	realloc@PLT
	movq	%rax, -16(%rbp)
	jmp	.L106
.L105:
	movq	-8(%rbp), %rax
	imulq	$4082, %rax, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -16(%rbp)
.L106:
	cmpq	$0, -16(%rbp)
	jne	.L107
	movq	-32(%rbp), %rax
	movl	$6, (%rax)
	movl	$0, %eax
	jmp	.L104
.L107:
	movq	-16(%rbp), %rax
.L104:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	allocate_macro_array_memory, .-allocate_macro_array_memory
	.globl	increment_macro_array_index
	.type	increment_macro_array_index, @function
increment_macro_array_index:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movq	%rdx, -40(%rbp)
	movq	$0, -8(%rbp)
	cmpl	$0, -28(%rbp)
	je	.L109
	movl	-28(%rbp), %ecx
	movslq	%ecx, %rax
	imulq	$1717986919, %rax, %rax
	shrq	$32, %rax
	movl	%eax, %edx
	sarl	$2, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	testl	%edx, %edx
	jne	.L109
	movq	-40(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	allocate_macro_array_memory
	movq	%rax, -8(%rbp)
.L109:
	movq	-8(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	increment_macro_array_index, .-increment_macro_array_index
	.globl	expand_macro
	.type	expand_macro, @function
expand_macro:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-20(%rbp), %eax
	cltq
	imulq	$4082, %rax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	leaq	32(%rax), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movq	%rdx, %rdi
	call	fputs@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	expand_macro, .-expand_macro
	.globl	is_newline_needed
	.type	is_newline_needed, @function
is_newline_needed:
.LFB14:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jne	.L113
	movl	$1, %eax
	jmp	.L114
.L113:
	cmpl	$19, -4(%rbp)
	jne	.L115
	movl	$1, %eax
	jmp	.L114
.L115:
	movl	$0, %eax
.L114:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	is_newline_needed, .-is_newline_needed
	.local	macro_multiplier_factor.2176
	.comm	macro_multiplier_factor.2176,1,1
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
