	.text
	.global mymain
mymain:
	movl $4, %eax
	movl $1, %ebx
	movl $msg, %ecx
	movl size, %edx
	int $0x80

	movl $1, %eax
	movl $0, %ebx
	int $0x80

msg:
	.ascii "Hello, World!\n"
size:
	.long . - msg
