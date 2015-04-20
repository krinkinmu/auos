#include <arch/memory.h>
#include <kernel/utility.h>
#include <kernel/acpi.h>
#include <kernel/debug.h>

static const void *acpi_find_rsdp_paddr_in_range(const void *from,
			const void *to)
{
	static const char RSDP_SIGN[] = "RSD PTR ";

	const char *l = (const char *)from;
	const char *r = (const char *)to;

	for (const char *ptr = l; ptr < r; ptr += 16) {
		if (!memcmp(RSDP_SIGN, ptr, sizeof(RSDP_SIGN) - 1))
			return ptr;
	}

	return 0;
}

static const void *acpi_find_rsdp_addr(void)
{
	const uintptr_t ebda_ptr =
			(uintptr_t)*((const uint16_t *)virt_addr(0x40eul << 4));
	const uintptr_t ebda_end = ebda_ptr + 0x400;

	const void *addr = acpi_find_rsdp_paddr_in_range(
			virt_addr(ebda_ptr), virt_addr(ebda_end));
	if (addr)
		return addr;

	return acpi_find_rsdp_paddr_in_range(
			virt_addr(0x0E0000ul), virt_addr(0x100000ul));
}

const struct acpi_table_rsdp *acpi_find_rsdp_table(void)
{
	const void *addr = acpi_find_rsdp_addr();

	if (addr)
		return (struct acpi_table_rsdp *)addr;
	return 0;
}

const struct acpi_table_rsdt *acpi_get_rsdt(const struct acpi_table_rsdp *rsdp)
{
	return virt_addr(rsdp->rsdt_paddr);
}

size_t acpi_table_headers_count(const struct acpi_table_rsdt *rsdt)
{
	const uint32_t *begin = rsdt->entries;
	const uint32_t *end =
		(const uint32_t *)((const char *)rsdt + rsdt->header.length);
	return end - begin;
}

const struct acpi_table_header *acpi_table_header_get(
			const struct acpi_table_rsdt *rsdt, size_t idx)
{
	return virt_addr(rsdt->entries[idx]);
}