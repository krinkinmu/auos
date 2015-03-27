#ifndef __MEMORY_H__
#define __MEMORY_H__

#define PHYS_MEM_MAX     0xFFFFFFFF
#define PAGE_OFFSET      0xC0000000
#define PAGE_SHIFT       12
#define PAGE_SIZE        (1 << PAGE_SHIFT)
#define PGDIR_SHIFT      22
#define PGDIR_SIZE       (1 << PGDIR_SHIFT)
#define PTRS_PER_PT      1024
#define PTRS_PER_PGD     1024

#ifndef ASM_FILE
#define pgd_index(ptr) \
	(((unsigned long)(ptr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

#define KERNEL_PGD_BOUNDARY pgd_index(PAGE_OFFSET)
#define KERNEL_PGD_PTRS     (PTRS_PER_PGD - KERNEL_PGD_BOUNDARY)
#define phys_addr(x)        ((unsigned long)(x) - PAGE_OFFSET)
#define virt_addr(x)        ((void *)((unsigned long)(x) + PAGE_OFFSET))

typedef unsigned long pgdval_t;
typedef unsigned long pteval_t;

typedef struct { pgdval_t pde; } pde_t;
typedef struct { pteval_t pte; } pte_t;

extern pde_t swapper_page_dir[PTRS_PER_PGD];
extern pde_t initial_page_dir[PTRS_PER_PGD];

extern char __kernel_begin[], __kernel_end[];

struct multiboot_info;
void setup_memory(struct multiboot_info *mbi);

static inline void load_cr3(unsigned long pde)
{ __asm__ ("movl %0, %%cr3" : : "a"(pde)); }

static inline pte_t page_table_entry(unsigned long phys, unsigned long flags)
{
	pte_t pte = { (phys & ~(PAGE_SIZE - 1)) | (flags & (PAGE_SIZE - 1)) };
	return pte;
}

static inline pde_t page_dir_entry(pte_t *ptr, unsigned long flags)
{
	pde_t pde = { phys_addr(ptr) | (flags & (PAGE_SIZE - 1)) };
	return pde;
}
#endif /* ASM_FILE */

#endif /*__MEMORY_H__*/
