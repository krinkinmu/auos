#ifndef __DEBUG_H__
#define __DEBUG_H__

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

#endif /*__DEBUG_H__*/
