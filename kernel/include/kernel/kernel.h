#ifndef __KERNEL_H__
#define __KERNEL_H__

#ifndef ASM_FILE

#include <stddef.h>

#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN_UP(x, a)      ALIGN_MASK(x, ((a) - 1))
#define ALIGN_DOWN(x, a)    ((x) & ~((a) - 1))

#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type, member)))


static inline unsigned long long minull(unsigned long long x,
			unsigned long long y)
{
	return x < y ? x : y;
}

static inline long long minll(long long x, long long y)
{
	return x < y ? x : y;
}

static inline unsigned long minul(unsigned long x, unsigned long y)
{
	return x < y ? x : y;
}

static inline long minl(long x, long y)
{
	return x < y ? x : y;
}

static inline unsigned minu(unsigned x, unsigned y)
{
	return x < y ? x : y;
}

static inline int mini(int x, int y)
{
	return x < y ? x : y;
}


static inline unsigned long long maxull(unsigned long long x,
			unsigned long long y)
{
	return x > y ? x : y;
}

static inline long long maxll(long long x, long long y)
{
	return x > y ? x : y;
}

static inline unsigned long maxul(unsigned long x, unsigned long y)
{
	return x > y ? x : y;
}

static inline long maxl(long x, long y)
{
	return x > y ? x : y;
}

static inline unsigned maxu(unsigned x, unsigned y)
{
	return x > y ? x : y;
}

static inline int maxi(int x, int y)
{
	return x > y ? x : y;
}


#define typed(name, expr) _Generic((expr), \
	unsigned long long: name ## ull, long long: name ## ll, \
	unsigned long: name ## ul, long: name ## l, \
	unsigned: name ## u, default: name ## i)

#define min(x, y) typed(min, 0 ? (x) : (y)) ((x), (y))
#define max(x, y) typed(max, 0 ? (x) : (y)) ((x), (y))

#endif

#ifdef ASM_FILE
#	define _AC(X, Y) X
#else
#	define _AC(X, Y) (X##Y)
#endif

#define BITUL(x)         (_AC(1,UL) << (x))
#define BITULL(x)        (_AC(1,ULL) << (x))

#endif /*__KERNEL_H__*/
