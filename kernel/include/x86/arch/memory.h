#ifndef __X86_ARCH_MEMORY_H__
#define __X86_ARCH_MEMORY_H__

#include <arch/asm/memory.h>
#include <arch/asm/cpu.h>

#include <stdint.h>
#include <stddef.h>

#define PTE_PRESENT   X86_PTE_PRESENT
#define PTE_USER      X86_PTE_USER
#define PTE_KERNEL    0UL
#define PTE_READ      0UL
#define PTE_NOREAD    0UL
#define PTE_WRITE     X86_PTE_WRITE
#define PTE_NOWRITE   0UL
#define PTE_CACHE     0UL
#define PTE_NOCHACHE  X86_PTE_NOCACHE

#define PDE_PRESENT   X86_PDE_PRESENT
#define PDE_USER      X86_PDE_USER
#define PDE_KERNEL    0UL
#define PDE_READ      0UL
#define PDE_NOREAD    0UL
#define PDE_WRITE     X86_PDE_WRITE
#define PDE_NOWRITE   0UL
#define PDE_CACHE     0UL
#define PDE_NOCACHE   X86_PDE_NOCACHE

#define pgd_index(ptr) \
	(((uintmax_t)(ptr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

#define phys_addr(x)        ((uintptr_t)(x) - PAGE_OFFSET)
#define virt_addr(x)        ((void *)((uintptr_t)(x) + PAGE_OFFSET))

typedef uint32_t pgdval_t;
typedef uint32_t pteval_t;

typedef struct { pgdval_t pde; } pde_t;
typedef struct { pteval_t pte; } pte_t;

extern pde_t swapper_page_dir[PTRS_PER_PGD];
extern pde_t initial_page_dir[PTRS_PER_PGD];
extern pte_t kernel_page_tables[KERNEL_PAGE_FRAMES];

extern char __kernel_begin[], __kernel_end[];

struct multiboot_info;
void setup_memory(struct multiboot_info *mbi);
void setup_page_alloc(void);
size_t page_frames(void);
size_t lowmem_page_frames(void);

static inline void load_cr3(unsigned long paddr)
{ __asm__ ("movl %0, %%cr3" : : "a"(paddr)); }

static inline unsigned long get_cr3(void)
{
	unsigned long ptr;
	__asm__ ("movl %%cr3, %0" : "=m"(ptr));
	return ptr;
}

static inline pte_t create_table_entry(unsigned long paddr, unsigned long flags)
{
	pte_t pt = { (paddr & ~PAGE_OFFSET_MASK) | (flags & PAGE_OFFSET_MASK) };
	return pt;
}

static inline pde_t create_dir_entry(unsigned long paddr, unsigned long flags)
{
	pde_t pd = { (paddr & ~PAGE_OFFSET_MASK) | (flags & PAGE_OFFSET_MASK) };
	return pd;
}

static inline unsigned long page_flags(pte_t pte)
{
	return pte.pte & PAGE_OFFSET_MASK;
}

static inline unsigned long page_paddr(pte_t pte)
{
	return pte.pte & ~PAGE_OFFSET_MASK;
}

#endif /*__X86_ARCH_MEMORY_H__*/
