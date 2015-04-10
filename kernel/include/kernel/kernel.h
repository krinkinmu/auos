#ifndef __KERNEL_KERNEL_H__
#define __KERNEL_KERNEL_H__

#ifndef ASM_FILE

#include <stddef.h>

static inline unsigned long alignul_mask(unsigned long x, unsigned long m)
{
	return (x + m) & ~m;
}

static inline unsigned long long alignull_mask(unsigned long long x,
			unsigned long long m)
{
	return (x + m) & ~m;
}

static inline unsigned long alignul_up(unsigned long x, unsigned long a)
{
	return alignul_mask(x, a - 1);
}

static inline unsigned long long alignull_up(unsigned long long x,
			unsigned long long a)
{
	return alignull_mask(x, a - 1);
}

static inline unsigned long alignul_down(unsigned long x, unsigned long a)
{
	return x & ~(a - 1);
}

static inline unsigned long long alignull_down(unsigned long long x,
			unsigned long long a)
{
	return x & ~(a - 1);
}

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
