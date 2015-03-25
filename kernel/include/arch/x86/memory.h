#ifndef __MEMORY_H__
#define __MEMORY_H__

#define PHYS_MEM_MAX 0xFFFFFFFF
#define PAGE_OFFSET  0xC0000000
#define PAGE_SHIFT   12
#define PAGE_SIZE    (1 << 12)
#define PGDIR_SHIFT  22
#define PGDIR_SIZE   (1 << 22)
#define PTRS_PER_PTE 1024
#define PTRS_PER_PGD 1024

#ifndef ASM_FILE
#define pgd_index(ptr) \
	(((unsigned long)(ptr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

#define KERNEL_PGD_BOUNDARY pgd_index(PAGE_OFFSET)
#define KERNEL_PGD_PTRS     (PTRS_PER_PGD - KERNEL_PGD_BOUNDARY)
#define phys_addr(x)        ((void *)((unsigned long)(x) - PAGE_OFFSET))

typedef unsigned long pgdval_t;
typedef unsigned long pteval_t;

typedef struct { pgdval_t pgd; } pgd_t;
typedef struct { pteval_t pte; } pte_t;

extern pgd_t swapper_page_dir[PTRS_PER_PGD];
extern pgd_t initial_page_dir[PTRS_PER_PGD];

static inline void load_cr3(pgd_t *pde)
{ __asm__ ("movl %0, %%cr3" : : "a"(pde)); }
#endif /* ASM_FILE */

#endif /*__MEMORY_H__*/
