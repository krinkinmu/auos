#ifndef __CPU_H__
#define __CPU_H__

#include <kernel/kernel.h>

#define X86_CR0_PE	BITUL(0)
#define X86_CR0_MP	BITUL(1)
#define X86_CR0_ET	BITUL(4)
#define X86_CR0_NE	BITUL(5)
#define X86_CR0_WP	BITUL(16)
#define X86_CR0_AM	BITUL(18)
#define X86_CR0_PG	BITUL(31)

#define X86_PTE_PRESENT   BITUL(0)
#define X86_PTE_WRITE     BITUL(1)
#define X86_PTE_USER      BITUL(2)
#define X86_PTE_WRTHROUGH BITUL(3)
#define X86_PTE_NOCACHE   BITUL(4)
#define X86_PTE_ACCESSED  BITUL(5)
#define X86_PTE_DIRTY     BITUL(6)
#define X86_PTE_GLOBAL    BITUL(8)

#define X86_PDE_PRESENT   BITUL(0)
#define X86_PDE_WRITE     BITUL(1)
#define X86_PDE_USER      BITUL(2)
#define X86_PDE_WRTHROUGH BITUL(3)
#define X86_PDE_NOCACHE   BITUL(4)
#define X86_PDE_ACCESSED  BITUL(5)
#define X86_PDE_LARGE     BITUL(7)

#endif /*__CPU_H__*/
