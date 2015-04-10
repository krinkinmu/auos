#ifndef __X86_ARCH_IOPORT_H__
#define __X86_ARCH_IOPORT_H__

void outb(unsigned short port, unsigned char b);
unsigned char inb(unsigned short port);
void outw(unsigned short port, unsigned short w);
unsigned short inw(unsigned short port);
void outl(unsigned short port, unsigned long l);
unsigned long inl(unsigned short port);

#endif /*__X86_ARCH_IOPORT_H__*/
