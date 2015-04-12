#ifndef __KERNEL_UTILITY_H__
#define __KERNEL_UTILITY_H__

#include <stddef.h>
#include <stdint.h>

void *memset(void *ptr, int value, size_t size);
void *memcpy(void *ptr, const void *src, size_t size);
int memcmp(const void *l, const void *r, size_t size);
void utoa(char *buf, uintptr_t d, unsigned base);
void itoa(char *buf, intptr_t d, unsigned base);

#endif /*__KERNEL_UTILITY_H__*/
