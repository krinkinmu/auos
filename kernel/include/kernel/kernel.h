#ifndef __KERNEL_KERNEL_H__
#define __KERNEL_KERNEL_H__

#include <kernel/asm/kernel.h>

#include <stdint.h>
#include <stddef.h>

static inline uintptr_t align_mask(uintptr_t x, uintptr_t m)
{
	return (x + m) & ~m;
}

static inline uintptr_t align_up(uintptr_t x, uintptr_t a)
{
	return align_mask(x, a - 1);
}

static inline uintptr_t align_down(uintptr_t x, uintptr_t a)
{
	return x & ~(a - 1);
}

static inline uintmax_t minu(uintmax_t x, uintmax_t y)
{
	return x < y ? x : y;
}

static inline intmax_t min(intmax_t x, intmax_t y)
{
	return x < y ? x : y;
}

static inline uintmax_t maxu(uintmax_t x, uintmax_t y)
{
	return x > y ? x : y;
}

static inline intmax_t max(intptr_t x, intmax_t y)
{
	return x > y ? x : y;
}


#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type, member)))

#define align(a) __attribute__((aligned(a)))

#endif /*__KERNEL_KERNEL_H__*/
