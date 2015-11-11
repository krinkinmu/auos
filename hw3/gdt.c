#include "defs.h"
#include "gdt.h"

/***********************************************************
 *        15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0  *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 15-0  |                   LIM [15:0]                  | *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 31-16 |                  BASE [15:0]                  | *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 47-32 |PR|Privl|SG|EX|DC|RW|AC|      BASE [23:16]     | *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 * 63-48 |      BASE[31:24]      |GR|SZ| 0| 0| LIM[19:16]| *
 *       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ *
 ***********************************************************/

#define GDT_4KBLOCK BITUL(23)
#define GDT_32BIT   BITUL(22)
#define GDT_PRESENT BITUL(15)
#define GDT_SEGMENT BITUL(12)
#define GDT_CODE    BITUL(11)
#define GDT_RDWR    BITUL(9)
#define GDT_EXRD    BITUL(9)

#define GDT_CODE_FLAGS (GDT_SEGMENT | GDT_PRESENT \
			| GDT_CODE | GDT_EXRD | GDT_4KBLOCK | GDT_32BIT)
#define GDT_DATA_FLAGS (GDT_SEGMENT | GDT_PRESENT \
			| GDT_RDWR | GDT_4KBLOCK | GDT_32BIT)

struct gdt_entry {
	unsigned long low;
	unsigned long high;
} __attribute__((packed));

struct gdt_ptr {
	unsigned short size;
	unsigned long base;
} __attribute__((packed));

static void setup_gdt_entry(struct gdt_entry *entry, unsigned long base,
			unsigned long limit, unsigned long flags)
{
	entry->low = (limit & 0xFFFFul) | ((base & 0xFFFFul) << 16);
	entry->high = ((base >> 16) & 0xFFul) | (base & 0xFF000000ul)
		| (limit & 0xF0000ul) | flags;
}

static void reload_segment_registers(void)
{
	__asm__ volatile (
		"mov %0, %%ds\r\n"
		"mov %0, %%es\r\n"
		"mov %0, %%ss\r\n"
		"ljmp %1, $1f\r\n"
		"1:"
		: : "a"(GDT_DATA_SELECTOR), "i"(GDT_CODE_SELECTOR));
}

static void set_gdt(const struct gdt_ptr *ptr)
{ __asm__ volatile ("lgdt (%0)" : : "a"(ptr)); }

void gdt_setup(void)
{
	static struct gdt_entry gdt[3];
	static struct gdt_ptr ptr;

	setup_gdt_entry(gdt, 0ul, 0ul, 0ul);
	setup_gdt_entry(&gdt[GDT_CODE_ENTRY], 0ul, 0xFFFFFul, GDT_CODE_FLAGS);
	setup_gdt_entry(&gdt[GDT_DATA_ENTRY], 0ul, 0xFFFFFul, GDT_DATA_FLAGS);

	ptr.size = sizeof(gdt) - 1;
	ptr.base = (unsigned long)gdt;

	set_gdt(&ptr);
	reload_segment_registers();
}
