OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start)

SECTIONS
{
	. = 1M + SIZEOF_HEADERS;
	.header : { *(.header) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }

	__bss_begin = .;
	.bss : { *(.bss) }
	. = ALIGN(0x1000);
	__bss_end = .;
}
