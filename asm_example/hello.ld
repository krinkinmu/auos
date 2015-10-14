OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(mymain)

SECTIONS
{
	. = 0x100000 + SIZEOF_HEADERS;
	.text : { *(.text) }
	.data : { *(.data) }
}
