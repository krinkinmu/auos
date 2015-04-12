#include <kernel/utility.h>

void * memset(void *ptr, int value, size_t size)
{
	const char *end = (char *)ptr + size;

        for (char *begin = ptr; begin != end; ++begin)
		*begin = value;

	return ptr;
}

void * memcpy(void *dst, const void *src, size_t size)
{
	const char *from = src;

	for (char *to = dst; size--; *to++ = *from++);

	return dst;
}

void utoa(char *buf, uintptr_t d, unsigned base)
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

void itoa(char *buf, intptr_t d, unsigned base)
{
	uintptr_t n = d;

	if (base == 10 && d < 0) {
		*buf++ = '-';
		n = (uintptr_t)(-d);
	}
	utoa(buf, n, base);
}
