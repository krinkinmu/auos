#include <arch/ioport.h>

void out8(uint16_t port, uint8_t b)
{
	__asm__("outb %0, %1" : : "a"(b), "d"(port));
}

uint8_t in8(uint16_t port)
{
	uint8_t b;
	__asm__("inb %1, %0" : "=a"(b) : "d"(port));
	return b;
}

void out16(uint16_t port, uint16_t w)
{
	__asm__("outw %0, %1" : : "a"(w), "d"(port));
}

uint16_t in16(uint16_t port)
{
	uint16_t w;
	__asm__("inw %1, %0" : "=a"(w) : "d"(port));
	return w;
}

void out32(uint16_t port, uint32_t l)
{
	__asm__("outl %0, %1" : : "a"(l), "d"(port));
}

uint32_t in32(uint16_t port)
{
	uint32_t l;
	__asm__("inl %1, %0" : "=a"(l) : "d"(port));
	return l;
}
