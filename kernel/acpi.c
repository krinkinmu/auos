#include <kernel/debug.h>
#include <kernel/acpi.h>
#include <kernel/utility.h>
#include <arch/memory.h>

static const struct acpi_rsdp *rsdp;
static const struct acpi_rsdt *rsdt;

void setup_acpi(void)
{
	rsdp = acpi_rsdp_probe();
	if (!rsdp) {
		debug("ACPI: probe failed\n");
		return;
	}
	rsdt = virt_addr(rsdp->rsdt_paddr);
}

int acpi_available(void)
{
	return rsdp != 0;
}

static unsigned long acpi_tables(void)
{
	const uint32_t *begin = rsdt->entries;
	const uint32_t *end = (uint32_t *)((char *)rsdt + rsdt->header.length);

	return end - begin;
}

const struct acpi_table_header* acpi_get_table(unsigned idx)
{
	return virt_addr(rsdt->entries[idx]);
}

const struct acpi_table_header* acpi_table_find(const char *sign)
{
	assert(acpi_available(), "ACPI: ACPI isn't initialized\n");

	const unsigned long tables = acpi_tables();

	for (unsigned i = 0; i != tables; ++i) {
		const struct acpi_table_header *hdr = acpi_get_table(i);

		if (!memcmp(hdr->signature, sign, ACPI_TABLE_SIGN_SIZE))
			return hdr;
	}
	return 0;
}

size_t acpi_parse_madt(enum acpi_madt_type type, madt_handler_t handler)
{
	assert(acpi_available(), "ACPI unavailable\n");
	const struct acpi_madt *tbl =
		(const void *)acpi_table_find(ACPI_MADT_SIGN);
	assert(tbl, "There is no MADT\n");

	const char *end = (const char *)tbl + tbl->header.length;
	const char *hdr = (const char *)(tbl + 1);
	size_t count = 0;

	while (hdr < end) {
		const struct acpi_madt_header *header = (const void *)hdr;

		if (header->type == type) {
			handler(header);
			++count;
		}
	}

	return count;
}
