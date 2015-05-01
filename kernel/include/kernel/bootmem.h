#ifndef __KERNEL_BOOT_MEM_ALLOCATOR_H__
#define __KERNEL_BOOT_MEM_ALLOCATOR_H__

int boot_mem_add(unsigned long begin, unsigned long end);
int boot_mem_reserve(unsigned long begin, unsigned long end);
unsigned long boot_mem_alloc(unsigned long size, unsigned align,
			unsigned long from, unsigned long to);
int boot_mem_is_free(unsigned long begin, unsigned long end);

#endif /*__KERNEL_BOOT_MEM_ALLOCATOR_H__*/
