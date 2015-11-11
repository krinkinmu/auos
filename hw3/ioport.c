void out8(unsigned short port, int value)
{ __asm__ volatile ("outb %0, %1" : : "a"((char)value), "d"(port)); }

int in8(unsigned short port)
{
	char byte;

	__asm__ volatile ("inb %1, %0" : "=a"(byte) : "d"(port));
	return byte;
}
