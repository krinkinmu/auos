OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

PAGE_OFFSET = 0xC0000000;
PAGE_SIZE   = 0x00001000;

SECTIONS
{
	/* GRUB will load kernel in memory starting from 1Mb */
	. = 1M;
	.header    : { *(.header) }
	.bootstrap : { *(.bootstrap) }

	. += PAGE_OFFSET;
	__kernel_begin = .;

	.text   : AT(ADDR(.text) - PAGE_OFFSET) { *(.text)   *(.text.*) }
	.rodata : { *(.rodata) *(.rodata.*) }
	.data   : { *(.data)   *(.data.*) }

	. = ALIGN(PAGE_SIZE);
	__bss_begin = .;
	.bss : { *(.bss) }

	. = ALIGN(PAGE_SIZE);
	__bss_end = .;
	__kernel_end = .;

	.debug_aranges 0 : { *(.debug_aranges) }
	.debug_info    0 : { *(.debug_info) }
	.debug_abbrev  0 : { *(.debug_abbrev) }
	.debug_line    0 : { *(.debug_line) }
        .debug_str     0 : { *(.debug_str) }

	/DISCARD/ : { *(.eh_frame .note.* .comment) }
}
