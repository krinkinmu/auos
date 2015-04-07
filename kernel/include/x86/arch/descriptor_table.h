#ifndef __X86_ARCH_DESCRIPTOR_TABLE_H__
#define __X86_ARCH_DESCRIPTOR_TABLE_H__

#include <kernel/kernel.h>
#include <kernel/debug.h>

#define SEGTYPE     BITULL(44)
#define READABLE    BITULL(41)
#define WRITEABLE   BITULL(41)
#define EXECUTABLE  BITULL(43)
#define USER        (BITULL(45) | BITULL(46))
#define PRESENT     BITULL(47)
#define PROTECTED   BITULL(54)
#define BLOCK4K     BITULL(55)

#define KERNEL_CODE_TYPE (SEGTYPE | PRESENT | EXECUTABLE \
			| READABLE | PROTECTED | BLOCK4K)
#define KERNEL_DATA_TYPE (SEGTYPE | PRESENT | WRITEABLE \
			| PROTECTED | BLOCK4K)

#define USER_CODE_TYPE (SEGTYPE | PRESENT | EXECUTABLE | USER \
			| READABLE | PROTECTED | BLOCK4K)
#define USER_DATA_TYPE (SEGTYPE | PRESENT | WRITEABLE \
			| USER | PROTECTED | BLOCK4K)

#define KERNEL_CODE_ENTRY    1
#define KERNEL_CODE_SELECTOR (KERNEL_CODE_ENTRY << 3)

#define KERNEL_DATA_ENTRY    2
#define KERNEL_DATA_SELECTOR (KERNEL_DATA_ENTRY << 3)

#define USER_CODE_ENTRY      3
#define USER_CODE_SELECTOR   (USER_CODE_ENTRY << 3)

#define USER_DATA_ENTRY      4
#define USER_DATA_SELECTOR   (USER_DATA_ENTRY << 3)

#define TYPE_MASK   (SEGTYPE | READABLE | WRITEABLE | EXECUTABLE \
			| USER | PRESENT | PROTECTED | BLOCK4K)
#define LIMIT_MASK  0x00F000000000FFFFULL
#define BASE_MASK   0xFF0000FFFFFF0000ULL

#define GDT_SIZE    8

struct descriptor_table_entry {
	unsigned long long data;
} __attribute__((packed));

struct descriptor_ptr {
	unsigned short size;
	unsigned long base;
} __attribute__((packed));

typedef struct descriptor_table_entry gdt_entry_t;
typedef struct descriptor_ptr gdt_descr_t;

void init_entry(struct descriptor_table_entry *entry, unsigned long long limit,
			unsigned long long base, unsigned long long type);
void init_descriptor_ptr(struct descriptor_ptr *ptr,
			struct descriptor_table_entry *table,
			unsigned long entries);
void set_gdt(struct descriptor_ptr *ptr);
struct descriptor_ptr get_gdt(void);
void reload_segment_registers(void);

#endif /*__X86_ARCH_DESCRIPTOR_TABLE_H__*/
