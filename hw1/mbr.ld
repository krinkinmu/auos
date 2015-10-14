OUTPUT_ARCH(i8086)
OUTPUT_FORMAT(binary)

SECTIONS
{
	. = 0x7c00;
	.text : { *(.text) *(.data) *(.rodata) }
	. = 0x7dfe;
	.signature : { SHORT(0xAA55) }
}
