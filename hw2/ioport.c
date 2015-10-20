void out8(unsigned short port, unsigned char value)
{ __asm__ volatile ("outb %0, %1" : : "a"(value), "d"(port)); }
