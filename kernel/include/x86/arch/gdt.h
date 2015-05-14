#ifndef __X86_ARCH_GDT_H__
#define __X86_ARCH_GDT_H__

#include <kernel/kernel.h>

#include <stddef.h>
#include <stdint.h>

#define GDT_PRESENT     BITUL(15)
#define GDT_USER        (BITUL(14) | BITUL(13))
#define GDT_NONSYSTEM   BITUL(12)
#define GDT_4KBLOCK     BITUL(23)
#define GDT_32BIT       BITUL(22)
#define GDT_CODE        BITUL(11)
#define GDT_WRITE       BITUL(9)
#define GDT_READ        BITUL(9)
#define GDT_TSS         BITUL(8)
#define GDT_32SYSTEM    BITUL(11)

#define KERNEL_CODE_TYPE (GDT_NONSYSTEM | GDT_PRESENT | GDT_CODE \
			| GDT_READ | GDT_4KBLOCK | GDT_32BIT)
#define KERNEL_DATA_TYPE (GDT_NONSYSTEM | GDT_PRESENT | GDT_WRITE \
			| GDT_4KBLOCK | GDT_32BIT)

#define USER_CODE_TYPE (GDT_NONSYSTEM | GDT_PRESENT | GDT_CODE \
			| GDT_READ | GDT_4KBLOCK | GDT_32BIT | GDT_USER)
#define USER_DATA_TYPE (GDT_NONSYSTEM | GDT_PRESENT | GDT_WRITE \
			| GDT_4KBLOCK | GDT_32BIT | GDT_USER)

#define FREE_TSS_TYPE (GDT_32SYSTEM | GDT_TSS | GDT_PRESENT)

#define GDT_SELECTOR(entry)  ((entry) << 3)
#define GDT_ENTRY(selector)  ((selector) >> 3)

#define KERNEL_CODE_ENTRY    1
#define KERNEL_CODE_SELECTOR GDT_SELECTOR(KERNEL_CODE_ENTRY)

#define KERNEL_DATA_ENTRY    2
#define KERNEL_DATA_SELECTOR GDT_SELECTOR(KERNEL_DATA_ENTRY)

#define USER_CODE_ENTRY      3
#define USER_CODE_SELECTOR   GDT_SELECTOR(USER_CODE_ENTRY)

#define USER_DATA_ENTRY      4
#define USER_DATA_SELECTOR   GDT_SELECTOR(USER_DATA_ENTRY)

#define TSS1_ENTRY           5
#define TSS1_SELECTOR        GDT_SELECTOR(TSS1_ENTRY)

#define TSS2_ENTRY           6
#define TSS2_SELECTOR        GDT_SELECTOR(TSS2_ENTRY)

#define GDT_SIZE    8

struct gdt_entry {
	uint64_t entry;
} __attribute__((packed));

struct gdt_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));

void init_gdt_segment(struct gdt_entry *entry, unsigned long limit,
			unsigned long base, unsigned long type);
void init_gdt_ptr(struct gdt_ptr *ptr, struct gdt_entry *table, size_t size);
void set_gdt(const struct gdt_ptr *ptr);
struct gdt_ptr get_gdt(void);
void reload_registers(void);

#endif /*__X86_ARCH_GDT_H__*/
