#ifndef __X86_ARCH_IOPORT_H__
#define __X86_ARCH_IOPORT_H__

#include <stdint.h>

void out8(uint16_t port, uint8_t b);
uint8_t in8(uint16_t port);
void out16(uint16_t port, uint16_t w);
uint16_t in16(uint16_t port);
void out32(uint16_t port, uint32_t l);
uint32_t in32(uint16_t port);

#endif /*__X86_ARCH_IOPORT_H__*/
