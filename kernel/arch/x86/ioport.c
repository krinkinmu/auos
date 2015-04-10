#include <arch/ioport.h>

void outb(unsigned short port, unsigned char b)
{
	__asm__("outb %0, %1" : : "a"(b), "d"(port));
}

unsigned char inb(unsigned short port)
{
	unsigned char b;
	__asm__("inb %1, %0" : "=a"(b) : "d"(port));
	return b;
}

void outw(unsigned short port, unsigned short w)
{
	__asm__("outw %0, %1" : : "a"(w), "d"(port));
}

unsigned short inw(unsigned short port)
{
	unsigned short w;
	__asm__("inw %1, %0" : "=a"(w) : "d"(port));
	return w;
}

void outl(unsigned short port, unsigned long l)
{
	__asm__("outl %0, %1" : : "a"(l), "d"(port));
}

unsigned long inl(unsigned short port)
{
	unsigned long l;
	__asm__("inl %1, %0" : "=a"(l) : "d"(port));
	return l;
}
