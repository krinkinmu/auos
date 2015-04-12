#include <kernel/acpi.h>
#include <kernel/debug.h>
#include <arch/memory.h>
#include <arch/multiboot.h>
#include <arch/early_console.h>

void setup_arch(void *bootstrap)
{
	struct multiboot_info *mbi = bootstrap;

	init_vga_console();
	init_serial_console();
	setup_memory(mbi);

	const struct acpi_table_rsdp *rsdp = acpi_find_rsdp_table();
	if (!rsdp) {
		debug("RSDP not found\n");
		return;
	}

	debug("Found RSDP Version %d\n", (int)rsdp->revision);
	const struct acpi_table_rsdt *rsdt = acpi_get_rsdt(rsdp);
	if ((uintptr_t)rsdt < PAGE_OFFSET) {
		debug("RSDT is out of low memory\n");
		return;
	}

	const size_t tables = acpi_table_headers_count(rsdt);
	debug("There are %u tables\n", (unsigned)tables);
	for (size_t i = 0; i != tables; ++i) {
		const struct acpi_table_header *header =
						acpi_table_header_get(rsdt, i);
		if ((uintptr_t)header < PAGE_OFFSET) {
			debug("Table %d is out of low memory\n", (unsigned)i);
			break;
		}

		debug("Table %d sign %c%c%c%c\n", (unsigned)i,
			header->signature[0],
			header->signature[1],
			header->signature[2],
			header->signature[3]);
	}
}
