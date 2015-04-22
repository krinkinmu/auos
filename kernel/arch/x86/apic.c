#include <kernel/acpi.h>
#include <kernel/debug.h>
#include <arch/apic.h>

#include <stddef.h>

static struct io_apic io_apic[16];
static size_t io_apic_size;

static void setup_io_apic_default(void)
{
	io_apic[0].vaddr = kmap(IO_APIC_DEFAULT_PADDR, IO_APIC_PAGE_FLAGS);
	io_apic[0].id = 0xF & (io_apic_read(&io_apic[0], IO_APIC_ID_REG) >> 24);
	io_apic[0].gsi_base = 0;
	++io_apic_size;

	debug("Default IO APIC at addr 0x%x id %u base %u\n",
		IO_APIC_DEFAULT_PADDR, io_apic[0].id, io_apic[0].gsi_base);
}

static int setup_io_apic_acpi(void)
{
	if (!acpi_available())
		return 0;

	const struct acpi_madt_table *tbl =
		(const struct acpi_madt_table *)acpi_table_find(ACPI_MADT_SIGN);
	if (!tbl)
		return 0;

	const char *begin = (const char *)tbl;
	const char *end = begin + tbl->header.length;
	const char *header = (const char *)(tbl + 1);

	while (header < end) {
		const struct acpi_irq_header *irq_hdr =
					(const struct acpi_irq_header *)header;
		header += irq_hdr->length;
		if (irq_hdr->type != MADT_IO_APIC)
			continue;

		const struct acpi_io_apic *apic =
					(const struct acpi_io_apic *)irq_hdr;
		io_apic[io_apic_size].vaddr = kmap(apic->io_apic_paddr,
					IO_APIC_PAGE_FLAGS);
		io_apic[io_apic_size].id = apic->io_apic_id;
		io_apic[io_apic_size].gsi_base = apic->gsi_base;
		debug("Found IO APIC at addr 0x%x id %u base %u\n",
			apic->io_apic_paddr, apic->io_apic_id, apic->gsi_base);
		++io_apic_size;
	}

	return 1;
}

uint32_t io_apic_read(const struct io_apic *apic, int reg)
{
	volatile uint32_t *io = apic->vaddr;
	io[0] = reg & 0xFF;
	return io[4];
}

void setup_io_apic(void)
{
	if (setup_io_apic_acpi())
		return;
	setup_io_apic_default();
}
