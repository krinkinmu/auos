#ifndef __KERNEL_ASM_KERNEL_H__
#define __KERNEL_ASM_KERNEL_H__

#ifdef ASM_FILE
#	define _AC(X, Y) X
#else
#	define _AC(X, Y) (X##Y)
#endif

#define BITU(x)          (_AC(1,U) << (x))
#define BITUL(x)         (_AC(1,UL) << (x))
#define BITULL(x)        (_AC(1,ULL) << (x))

#endif /*__KERNEL_ASM_KERNEL_H__*/
