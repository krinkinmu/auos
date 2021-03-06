#ifndef __BOOT_H__
#define __BOOT_H__

#include <arch/asm/memory.h>
#include <arch/asm/cpu.h>

/* multiboot definitions */
#define MB_HEADER_MAGIC      0x1BADB002
#define MB_BOOT_MAGIC        0x2BADB002
#define MB_PAGE_ALIGN        0x00000001
#define MB_MEMORY_INFO       0x00000002

#define BOOT_CS_ENTRY        1
#define BOOT_CS              (BOOT_CS_ENTRY << 3)
#define BOOT_DS_ENTRY        2
#define BOOT_DS              (BOOT_DS_ENTRY << 3)

#define PTE_INIT_ATTR        0x00000003
#define PDE_INIT_ATTR        0x00000003

#define pa(x)                ((x) - PAGE_OFFSET)

#endif /*__BOOT_H__*/
