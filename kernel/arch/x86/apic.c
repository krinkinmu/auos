#include <kernel/acpi.h>
#include <kernel/debug.h>
#include <arch/apic.h>
#include <arch/pic.h>

#include <stddef.h>

static struct io_apic io_apic[16];
static size_t io_apic_size;

static uint32_t io_apic_read(void *vaddr, unsigned reg)
{
	volatile uint32_t *io = vaddr;
	io[0] = reg & 0xFFU;
	return io[4];
}

static unsigned io_apic_irqs_count(void *vaddr)
{
	return 1 + ((io_apic_read(vaddr, IO_APIC_VERSION_REG) >> 16) & 0xFFU);
}

static void setup_io_apic_default(void)
{
	void *io_base = kmap(IO_APIC_DEFAULT_PADDR, IO_APIC_PAGE_FLAGS);
	io_apic[0].vaddr = io_base;
	io_apic[0].gsi_begin = 0;
        io_apic[0].gsi_end = io_apic_irqs_count(io_base);
	++io_apic_size;

	debug("Default IO APIC at addr 0x%x irqs [%u, %u)\n",
			IO_APIC_DEFAULT_PADDR,
			io_apic[0].gsi_begin,
			io_apic[0].gsi_end);
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

		void *io_base = kmap(apic->io_apic_paddr, IO_APIC_PAGE_FLAGS);
		const unsigned gsi_begin = apic->gsi_base;
		const unsigned gsi_count = io_apic_irqs_count(io_base);

		io_apic[io_apic_size].vaddr = io_base;
		io_apic[io_apic_size].gsi_begin = gsi_begin;
		io_apic[io_apic_size].gsi_end = gsi_begin + gsi_count;

		debug("Found IO APIC at addr 0x%x base [%u, %u)\n",
				apic->io_apic_paddr,
				gsi_begin,
				gsi_begin + gsi_count);
		++io_apic_size;
	}

	return 1;
}

void setup_io_apic(void)
{
	if (setup_io_apic_acpi())
		return;
	setup_io_apic_default();

	pic_remap(0x20U);
	pic_mask_interrupts(0xFFU);
}
