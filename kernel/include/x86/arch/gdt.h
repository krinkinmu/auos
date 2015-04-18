#ifndef __X86_ARCH_GDT_H__
#define __X86_ARCH_GDT_H__

#include <kernel/kernel.h>

#include <stddef.h>
#include <stdint.h>

#define GDT_SEGMENT     BITU(4)
#define GDT_READABLE    BITU(1)
#define GDT_WRITEABLE   BITU(1)
#define GDT_EXECUTABLE  BITU(3)
#define GDT_USER        (BITU(5) | BITU(6))
#define GDT_PRESENT     BITU(7)
#define GDT_PROTECTED   BITU(14)
#define GDT_BLOCK4K     BITU(15)
#define GDT_TSSTYPE     (BITU(0) | BITU(3))
#define GDT_BUSYTSS     BITU(1)

#define KERNEL_CODE_TYPE (GDT_SEGMENT | GDT_PRESENT | GDT_EXECUTABLE \
			| GDT_READABLE | GDT_PROTECTED | GDT_BLOCK4K)
#define KERNEL_DATA_TYPE (GDT_SEGMENT | GDT_PRESENT | GDT_WRITEABLE \
			| GDT_PROTECTED | GDT_BLOCK4K)

#define USER_CODE_TYPE (GDT_SEGMENT | GDT_PRESENT | GDT_EXECUTABLE \
			| GDT_USER | GDT_READABLE | GDT_PROTECTED \
			| GDT_BLOCK4K)
#define USER_DATA_TYPE (GDT_SEGMENT | GDT_PRESENT | GDT_WRITEABLE \
			| GDT_USER | GDT_PROTECTED | GDT_BLOCK4K)

#define BUSY_TSS_TYPE (GDT_TSSTYPE | GDT_PRESENT | GDT_BUSYTSS)
#define FREE_TSS_TYPE (GDT_TSSTYPE | GDT_PRESENT)

#define KERNEL_CODE_ENTRY    1
#define KERNEL_CODE_SELECTOR (KERNEL_CODE_ENTRY << 3)

#define KERNEL_DATA_ENTRY    2
#define KERNEL_DATA_SELECTOR (KERNEL_DATA_ENTRY << 3)

#define USER_CODE_ENTRY      3
#define USER_CODE_SELECTOR   (USER_CODE_ENTRY << 3)

#define USER_DATA_ENTRY      4
#define USER_DATA_SELECTOR   (USER_DATA_ENTRY << 3)

#define TSS1_ENTRY           5
#define TSS1_SELECTOR        (TSS1_ENTRY << 3)

#define TSS2_ENTRY           6
#define TSS2_SELECTOR        (TSS2_ENTRY << 3)

#define GDT_SIZE    8

struct gdt_entry {
	uint64_t entry;
} __attribute__((packed));

struct gdt_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));

void init_gdt_segment(struct gdt_entry *entry, uint32_t limit,
			uint32_t base, unsigned type);
void init_gdt_ptr(struct gdt_ptr *ptr, struct gdt_entry *table, size_t size);
void set_gdt(const struct gdt_ptr *ptr);
struct gdt_ptr get_gdt(void);
void reload_registers(void);

#endif /*__X86_ARCH_GDT_H__*/
