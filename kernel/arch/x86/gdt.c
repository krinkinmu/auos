#include <kernel/debug.h>
#include <arch/gdt.h>

void init_gdt_segment(struct gdt_entry *entry, uint32_t limit,
			uint32_t base, unsigned type)
{
	warn_if(limit > 0xFFFFFul, "Limit overflow\n");
	warn_if(base > 0xFFFFFFFFul, "Base overflow\n");

	const uint64_t descr = ((uint64_t)limit & 0x0FFFFul) |
				(((uint64_t)limit & 0xF0000ul) << 32) |
				(((uint64_t)base & 0x00FFFFFFul) << 16) |
				(((uint64_t)base & 0xFF000000ul) << 32) |
				(((uint64_t)type & 0xF0FFul) << 40);

	entry->entry = descr;
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
