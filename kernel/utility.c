#include <kernel/utility.h>

void * memset(void *ptr, int value, unsigned long size)
{
	const char *end = (char *)ptr + size;

        for (char *begin = ptr; begin != end; ++begin)
		*begin = value;

	return ptr;
}

void * memcpy(void *dst, const void *src, unsigned long size)
{
	const char *from = src;

	for (char *to = dst; size--; *to++ = *from++);

	return dst;
}

void ultoa(char *buf, unsigned long d, int base)
{
	char *begin = buf;
	char *end = buf;

	do {
		unsigned rem = d % base;
		*end++ = (rem < 10) ? rem + '0' : rem + 'a' - 10;
	} while (d /= base);

	*end-- = 0;
	while (begin < end) {
		char tmp = *begin;
		*begin++ = *end;
		*end-- = tmp;
	}
}

void ltoa(char *buf, long d, int base)
{
	unsigned long n = d;

	if (base == 10 && d < 0) {
		*buf++ = '-';
		n = (unsigned long)(-d);
	}
	ultoa(buf, n, base);
}
