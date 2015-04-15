#ifndef __X86_ARCH_MEMORY_H__
#define __X86_ARCH_MEMORY_H__

#include <arch/asm/memory.h>

#include <stdint.h>
#include <stddef.h>

#define PAGE_FRAMES_MAX     (1 + (PHYS_MEM_MAX >> PAGE_SHIFT))
#define LOW_PAGE_FRAMES_MAX (1 + ((PHYS_MEM_MAX - PAGE_OFFSET) >> PAGE_SHIFT))

#define pgd_index(ptr) \
	(((uintptr_t)(ptr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

#define KERNEL_PGD_BOUNDARY pgd_index(PAGE_OFFSET)
#define KERNEL_PGD_PTRS     (PTRS_PER_PGD - KERNEL_PGD_BOUNDARY)
#define phys_addr(x)        ((uintptr_t)(x) - PAGE_OFFSET)
#define virt_addr(x)        ((void *)((uintptr_t)(x) + PAGE_OFFSET))

typedef uint32_t pgdval_t;
typedef uint32_t pteval_t;

typedef struct { pgdval_t pde; } pde_t;
typedef struct { pteval_t pte; } pte_t;

extern pde_t swapper_page_dir[PTRS_PER_PGD];
extern pde_t initial_page_dir[PTRS_PER_PGD];

extern char __kernel_begin[], __kernel_end[];

extern size_t page_frames, low_mem_page_frames;

struct multiboot_info;
void setup_memory(struct multiboot_info *mbi);
void setup_page_alloc(void);

static inline void load_cr3(uintptr_t pde)
{ __asm__ ("movl %0, %%cr3" : : "a"(pde)); }

static inline pte_t page_table_entry(uint32_t phys, uint32_t flags)
{
	pte_t pte = { (phys & ~(PAGE_SIZE - 1)) | (flags & (PAGE_SIZE - 1)) };
	return pte;
}

static inline pde_t page_dir_entry(pte_t *ptr, uint32_t flags)
{
	pde_t pde = { phys_addr(ptr) | (flags & (PAGE_SIZE - 1)) };
	return pde;
}

#endif /*__X86_ARCH_MEMORY_H__*/
