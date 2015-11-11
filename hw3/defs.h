#ifndef __COMMON_DEFINITIONS_H__
#define __COMMON_DEFINITIONS_H__

#include <stddef.h>

#define BITU(x)   (1u << (x))
#define BITUL(x)  (1ul << (x))
#define BITULL(x) (1ull << (x))

#define container_of(ptr, type, member) \
        (type *)((char *)(ptr) - offsetof(type, member))

#endif /*__COMMON_DEFINTIONS_H__*/
