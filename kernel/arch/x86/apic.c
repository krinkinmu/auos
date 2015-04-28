#include <kernel/acpi.h>
#include <kernel/debug.h>
#include <arch/apic.h>

#include <stddef.h>

static struct io_apic apic_io_apic[IO_APIC_MAX_COUNT];
static size_t apic_io_apic_count;

static struct local_apic apic_local_apic[LOCAL_APIC_MAX_COUNT];
static size_t apic_local_apic_count;

static struct isa_irq_map apic_isa_map[ISA_IRQ_MAX_COUNT];

static uint32_t io_apic_read(void *vaddr, unsigned reg)
{
	volatile uint32_t *io = vaddr;
	io[0] = reg & 0xFFU;
	return io[4];
}

static void io_apic_write(void *vaddr, unsigned reg, uint32_t value)
{
	volatile uint32_t *io = vaddr;
	io[0] = reg & 0xFFU;
	io[4] = value;
}

static unsigned apic_io_apic_irqs(void *vaddr)
{
	return 1 + ((io_apic_read(vaddr, IO_APIC_VERSION_REG) >> 16) & 0xFFU);
}

static unsigned apic_io_apic_id(void *vaddr)
{
	return (io_apic_read(vaddr, IO_APIC_ID_REG) >> 24) & 0x0FU;
}

static void apic_parse_irq_source_override(
			const struct acpi_irq_source_override *descr)
{
	if (descr->source == descr->gsi && !(descr->flags &
				(ISA_POLARITY_MASK | ISA_TRIGGER_MASK)))
		return;

	struct isa_irq_map map = {
		descr->gsi,
		IO_APIC_IRQ_DEST_LOGICAL | IO_APIC_IRQ_LOW_PRIORITY
	};

	if ((descr->flags & ISA_POLARITY_MASK) == ISA_POLARITY_LOW)
		map.flags |= IO_APIC_IRQ_POLARITY_LOW;
	else
		map.flags |= IO_APIC_IRQ_POLARITY_HIGH;	

	if ((descr->flags & ISA_TRIGGER_MASK) == ISA_TRIGGER_LEVEL)
		map.flags |= IO_APIC_IRQ_TRIGGER_LEVEL;
	else
		map.flags |= IO_APIC_IRQ_TRIGGER_EDGE;

	apic_isa_map[descr->source] = map;

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

static void apic_setup_irq(struct io_apic *apic, unsigned pin, unsigned dst,
			unsigned long flags, unsigned cpus)
{
	const uint32_t low = (dst & 0xFFU) | (flags & 0xFF00UL);
	const uint32_t high = ((uint32_t)cpus & 0xFFU) << 24;

	io_apic_write(apic->vaddr, IO_APIC_REDTBL_REG + 2 * pin + 1, high);
	io_apic_write(apic->vaddr, IO_APIC_REDTBL_REG + 2 * pin, low);
}

static void apic_setup_isa_irqs_default(void)
{
	for (unsigned i = 0; i != ISA_IRQ_MAX_COUNT; ++i) {
		struct isa_irq_map map = {
			i,
			IO_APIC_IRQ_POLARITY_HIGH |
			IO_APIC_IRQ_TRIGGER_EDGE |
			IO_APIC_IRQ_DEST_LOGICAL |
			IO_APIC_IRQ_LOW_PRIORITY
		};
		apic_isa_map[i] = map;
	}
}

static struct io_apic *apic_find_gsi(unsigned gsi)
{
	for (unsigned i = 0; i != apic_io_apic_count; ++i) {
		struct io_apic *apic = &apic_io_apic[i];
		if (apic->gsi_begin <= gsi && apic->gsi_end > gsi)
			return apic;
	}

	assert(0, "Unexpected GSI\n");
	return 0;
}

static void apic_setup_irqs(void)
{
	for (unsigned i = 0; i != ISA_IRQ_MAX_COUNT; ++i) {
		struct io_apic *apic = apic_find_gsi(apic_isa_map[i].gsi);
		const unsigned pin = apic_isa_map[i].gsi - apic->gsi_begin;
		apic_setup_irq(apic, pin, i + 0x20, apic_isa_map[i].flags,
			IO_APIC_DEST_ALL);
	}
}

void setup_apic(void)
{
	assert(acpi_available(), "ACPI inavailable!\n");
	const struct acpi_madt_table *tbl =
				(const void *)acpi_table_find(ACPI_MADT_SIGN);
	assert(tbl, "There is no MADT!");
	apic_setup_isa_irqs_default();
	apic_parse_madt(tbl);
	apic_setup_irqs();
}
