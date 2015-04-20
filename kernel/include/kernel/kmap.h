#ifndef __KERNEL_KMAP_H__
#define __KERNEL_KMAP_H__

#include <arch/memory.h>

void *kmap(unsigned long paddr, unsigned long flags);
void kunmap(const void *vaddr);

#endif /*__KERNEL_TEMP_MAP_H__*/
