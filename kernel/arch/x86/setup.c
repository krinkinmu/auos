#include "multiboot.h"
#include "io.h"

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;
	unsigned long mmap_entry = mbi->mmap_addr;
	unsigned long mmap_end = mmap_entry + mbi->mmap_length;

	while (mmap_entry < mmap_end) {
		struct multiboot_mmap_entry *entry = (void *)mmap_entry;
		unsigned long addr = entry->addr;
		unsigned long size = entry->len;
		unsigned long type = entry->type;

		printf("memory region: 0x%x-0x%x type=%u\n",
			addr, addr + size - 1, type);
		mmap_entry += entry->size + sizeof(entry->size);
	}
}

void main(void *bootstrap)
{
	init_io();
	puts("Hello, World!");
	setup_arch(bootstrap);
	while (1);
}
