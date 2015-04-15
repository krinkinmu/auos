#ifndef __X86_ARCH_DESCRIPTOR_TABLE_H__
#define __X86_ARCH_DESCRIPTOR_TABLE_H__

#include <kernel/kernel.h>
#include <kernel/debug.h>

#include <stddef.h>
#include <stdint.h>

#define SEGMENT     BITU(4)
#define READABLE    BITU(1)
#define WRITEABLE   BITU(1)
#define EXECUTABLE  BITU(3)
#define USER        (BITU(5) | BITU(6))
#define PRESENT     BITU(7)
#define PROTECTED   BITU(14)
#define BLOCK4K     BITU(15)
#define TSSTYPE     (BITU(0) | BITU(3))
#define BUSYTSS     BITU(1)

#define KERNEL_CODE_TYPE (SEGMENT | PRESENT | EXECUTABLE \
			| READABLE | PROTECTED | BLOCK4K)
#define KERNEL_DATA_TYPE (SEGMENT | PRESENT | WRITEABLE \
			| PROTECTED | BLOCK4K)

#define USER_CODE_TYPE (SEGMENT | PRESENT | EXECUTABLE | USER \
			| READABLE | PROTECTED | BLOCK4K)
#define USER_DATA_TYPE (SEGMENT | PRESENT | WRITEABLE \
			| USER | PROTECTED | BLOCK4K)

#define BUSY_TSS_TYPE (TSSTYPE | PRESENT | BUSYTSS)
#define FREE_TSS_TYPE (TSSTYPE | PRESENT)

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

struct descriptor_table_entry {
	uint64_t data;
} __attribute__((packed));

struct descriptor_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));

typedef struct descriptor_table_entry gdt_entry_t;
typedef struct descriptor_ptr gdt_descr_t;

void init_entry(struct descriptor_table_entry *entry, uint32_t limit,
			uint32_t base, unsigned type);
void init_descriptor_ptr(struct descriptor_ptr *ptr,
			struct descriptor_table_entry *table,
			size_t entries);
void set_gdt(struct descriptor_ptr *ptr);
struct descriptor_ptr get_gdt(void);
void reload_segment_registers(void);

#endif /*__X86_ARCH_DESCRIPTOR_TABLE_H__*/
