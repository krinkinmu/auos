#ifndef __X86_ARCH_ASM_MEMORY_H__
#define __X86_ARCH_ASM_MEMORY_H__

#define PHYS_MEM_MAX     0xFFFFFFFF
#define PAGE_OFFSET      0xC0000000
#define PAGE_SHIFT       12
#define PAGE_SIZE        (1 << PAGE_SHIFT)
#define PGDIR_SHIFT      22
#define PGDIR_SIZE       (1 << PGDIR_SHIFT)
#define PTRS_PER_PT      1024
#define PTRS_PER_PGD     1024

#endif /*__X86_ARCH_ASM_MEMORY_H__*/
