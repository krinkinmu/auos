#include <kernel/acpi.h>
#include <kernel/debug.h>
#include <arch/apic.h>

#include <stddef.h>

static struct io_apic apic_io_apic[IO_APIC_MAX_COUNT];
static size_t apic_io_apic_count;

static struct local_apic apic_local_apic[LOCAL_APIC_MAX_COUNT];
static size_t apic_local_apic_count;

static uint32_t apic_read(void *vaddr, unsigned reg)
{
	volatile uint32_t *io = vaddr;
	io[0] = reg & 0xFFU;
	return io[4];
}

static unsigned apic_io_apic_irqs(void *vaddr)
{
	return 1 + ((apic_read(vaddr, IO_APIC_VERSION_REG) >> 16) & 0xFFU);
}

static unsigned apic_io_apic_id(void *vaddr)
{
	return (apic_read(vaddr, IO_APIC_ID_REG) >> 24) & 0x0FU;
}

static void apic_parse_irq_source_override(
			const struct acpi_irq_source_override *descr)
{
	debug("Standard ISA irq %u connected to %u gsi, flags %x\n",
		descr->source,
		descr->gsi,
		descr->flags);
}

static void apic_parse_io_apic(const struct acpi_io_apic *apic)
{
	if (apic_io_apic_count == IO_APIC_MAX_COUNT) {
		debug("Ignore IO APIC [%u]\n", apic->apic_id);
		return;
	}

	void *io_base = kmap(apic->apic_paddr, IO_APIC_PAGE_FLAGS);
	const unsigned gsi_begin = apic->gsi_base;
	const unsigned gsi_end = gsi_begin + apic_io_apic_irqs(io_base);
	const unsigned id = apic_io_apic_id(io_base);

	assert(apic->apic_id == id,
		"ACPI reported IO APIC id doesn't match IO APIC id\n");

	apic_io_apic[apic_io_apic_count].vaddr = io_base;
	apic_io_apic[apic_io_apic_count].gsi_begin = gsi_begin;
	apic_io_apic[apic_io_apic_count].gsi_end = gsi_end;
	apic_io_apic[apic_io_apic_count].id = id;
	++apic_io_apic_count;

	debug("Found IO APIC[%u] at addr 0x%x irqs [%u, %u)\n",
		id,
		apic->apic_paddr,
		gsi_begin,
		gsi_end);
}

static void apic_parse_local_apic(const struct acpi_local_apic *apic)
{
	if (apic_local_apic_count == LOCAL_APIC_MAX_COUNT) {
		debug("Ignore IO APIC [%u]\n", apic->apic_id);
		return;
	}

	apic_local_apic[apic_local_apic_count].id = apic->apic_id;
	++apic_local_apic_count;
	debug("Found Local APIC[%u]\n", apic->apic_id);
}

static void apic_parse_madt(const struct acpi_madt_table *tbl)
{
	const char *end = (const char *)tbl + tbl->header.length;
	const char *irq_ptr = (const char *)(tbl + 1);

	while (irq_ptr < end) {
		const struct acpi_irq_header *header = (const void *)irq_ptr;
		irq_ptr += header->length;
		switch (header->type) {
		case MADT_IO_APIC:
			apic_parse_io_apic((const void *)header);
			break;
		case MADT_LOCAL_APIC:
			apic_parse_local_apic((const void *)header);
			break;
		case MADT_IRQ_SRC_OVERRIDE:
			apic_parse_irq_source_override((const void *)header);
			break;
		default:
			debug("Unknown MADT header %u\n", header->type);
			break;
		}
	}

	void *local_apic_vaddr = kmap(tbl->lapic_paddr, LOCAL_APIC_PAGE_FLAGS);
	for (unsigned i = 0; i != apic_local_apic_count; ++i)
		apic_local_apic[i].vaddr = local_apic_vaddr;
}

void setup_apic(void)
{
	assert(acpi_available(), "ACPI inavailable!\n");
	const struct acpi_madt_table *tbl =
				(const void *)acpi_table_find(ACPI_MADT_SIGN);
	assert(tbl, "There is no MADT!");
	apic_parse_madt(tbl);
}
