#include <kernel/debug.h>
#include <arch/descriptor_table.h>

void init_entry(struct descriptor_table_entry *entry, uint32_t limit,
			uint32_t base, unsigned type)
{
	warn_if(limit > 0xFFFFFul, "Limit overflow\n");
	warn_if(base > 0xFFFFFFFFul, "Base overflow\n");

	const uint64_t descr = ((uint64_t)limit & 0x0FFFFul) |
				(((uint64_t)limit & 0xF0000ul) << 32) |
				(((uint64_t)base & 0x00FFFFFFul) << 16) |
				(((uint64_t)base & 0xFF000000ul) << 32) |
				(((uint64_t)type & 0xF0FFul) << 40);

	entry->data = descr;
}

void init_descriptor_ptr(struct descriptor_ptr *ptr,
			struct descriptor_table_entry *table,
			size_t entries)
{
	ptr->size = sizeof(*table) * entries - 1;
	ptr->base = (uint32_t)table;
}

void set_gdt(struct descriptor_ptr *ptr)
{
	__asm__ ("lgdt (%0)" : : "a"(ptr));
}

struct descriptor_ptr get_gdt(void)
{
	struct descriptor_ptr descr;
	__asm__ ("sgdt (%0)" : : "a"(&descr));
	return descr;
}

void reload_segment_registers(void)
{
	__asm__ (
		"mov %0, %%ds\r\n"
		"mov %0, %%es\r\n"
		"mov %0, %%ss\r\n"
		"ljmp %1, $1f\r\n"
		"1:" 
		: : "a"(KERNEL_DATA_SELECTOR), "i"(KERNEL_CODE_SELECTOR)); 
}
