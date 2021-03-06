#ifndef __KERNEL_DEBUG_H__
#define __KERNEL_DEBUG_H__

#include <kernel/io.h>

#ifdef NDEBUG
#define debug(...)
#else
#define debug(...)                   \
	do {                         \
		printf(__VA_ARGS__); \
	} while(0)
#endif

#define panic(...)                   \
	do {                         \
		printf(__VA_ARGS__); \
		while (1);           \
	} while (0)

#define warn_if(expr, ...)           \
	if (expr)                    \
		printf(__VA_ARGS__)

#define assert(expr, msg)            \
        if (!(expr))                 \
		panic(msg)

#endif /*__KERNEL_DEBUG_H__*/
