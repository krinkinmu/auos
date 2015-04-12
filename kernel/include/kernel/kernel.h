#ifndef __KERNEL_KERNEL_H__
#define __KERNEL_KERNEL_H__

#ifndef ASM_FILE

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

static inline intmax_t maxl(intptr_t x, intmax_t y)
{
	return x > y ? x : y;
}


#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type, member)))

#endif

#ifdef ASM_FILE
#	define _AC(X, Y) X
#else
#	define _AC(X, Y) (X##Y)
#endif

#define BITU(x)          (_AC(1,U) << (x))
#define BITUL(x)         (_AC(1,UL) << (x))
#define BITULL(x)        (_AC(1,ULL) << (x))

#endif /*__KERNEL_KERNEL_H__*/
