#include <kernel/debug.h>
#include <arch/gdt.h>

void init_gdt_segment(struct gdt_entry *entry, unsigned long limit,
			unsigned long base, unsigned long type)
{
	warn_if(limit > 0xFFFFFUL, "Limit overflow\n");
	warn_if(base > 0xFFFFFFFFUL, "Base overflow\n");

	const uint32_t low = (limit & 0xFFFFUL) | ((base & 0xFFFFUL) << 16);
	const uint32_t high =
		(base & 0xFF000000UL) | ((base & 0xFF0000UL) >> 16) |
		(limit & 0xF0000UL) | (type & 0xF0FF00UL);

	entry->entry = ((uint64_t)high << 32) | low;
}

void init_gdt_ptr(struct gdt_ptr *ptr, struct gdt_entry *table, size_t size)
{
	ptr->size = sizeof(*table) * size - 1;
	ptr->base = (uint32_t)table;
}

void set_gdt(const struct gdt_ptr *ptr)
{
	__asm__ ("lgdt (%0)" : : "a"(ptr));
}

struct gdt_ptr get_gdt(void)
{
	struct gdt_ptr ptr;
	__asm__ ("sgdt %0" : "=m"(ptr));
	return ptr;
}

void reload_registers(void)
{
	__asm__ (
		"mov %0, %%ds\r\n"
		"mov %0, %%es\r\n"
		"mov %0, %%ss\r\n"
		"ljmp %1, $1f\r\n"
		"1:" 
		: : "a"(KERNEL_DATA_SELECTOR), "i"(KERNEL_CODE_SELECTOR)); 
}
