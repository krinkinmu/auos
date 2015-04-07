#include <arch/descriptor_table.h>

void init_entry(struct descriptor_table_entry *entry, unsigned long long limit,
			unsigned long long base, unsigned long long type)
{
	warn_if(limit > 0xFFFFFULL, "Limit overflow\n");
	warn_if(base > 0xFFFFFFFFULL, "Base overflow\n");

	entry->data = ((limit & 0xF0000ULL) << 32) | (limit & 0xFFFFULL)
		| ((base & 0xFFFFFFULL) << 16) | ((base & 0xFF000000ULL) << 32)
		| type;
}

void init_descriptor_ptr(struct descriptor_ptr *ptr,
			struct descriptor_table_entry *table,
			unsigned long entries)
{
	ptr->size = sizeof(*table) * entries - 1;
	ptr->base = (unsigned long)table;
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
