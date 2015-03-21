#ifndef __CPU_H__
#define __CPU_H__

#ifdef ASM_FILE
#define _AC(X, Y)	X
#else
#define _AC(X, Y)	(X##Y)
#endif

#define BITUL(x)	(_AC(1,UL) << (x))

#define X86_CR0_PE	BITUL(0)
#define X86_CR0_MP	BITUL(1)
#define X86_CR0_ET	BITUL(4)
#define X86_CR0_NE	BITUL(5)
#define X86_CR0_WP	BITUL(16)
#define X86_CR0_AM	BITUL(18)
#define X86_CR0_PG	BITUL(31)

#endif /*__CPU_H__*/
