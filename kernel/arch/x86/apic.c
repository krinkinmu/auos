#include <kernel/acpi.h>
#include <kernel/debug.h>
#include <kernel/irq.h>
#include <kernel/kmap.h>
#include <kernel/utility.h>

#include <arch/apic.h>


#define APIC_PAGE_FLAGS       (PTE_KERNEL | PTE_READ | PTE_WRITE | PTE_NOCACHE)

#define ISA_IRQ_COUNT         16
#define ISA_POLARITY_MASK     0x03U
#define ISA_POLARITY_HIGH     0x01U
#define ISA_POLARITY_LOW      0x03U
#define ISA_TRIGGER_MASK      0x0CU
#define ISA_TRIGGER_LEVEL     0x0CU
#define ISA_TRIGGER_EDGE      0x04U

#define IO_APIC_MAX_COUNT     32
#define IO_APIC_ID_REG        0x00U
#define IO_APIC_VER_REG       0x01U
#define IO_APIC_REDTBL_REG    0x10U
#define IO_APIC_PAGE_FLAGS    APIC_PAGE_FLAGS
#define IO_APIC_DEST_ALL      0xFFU
#define IO_APIC_LOW_PRI       BITUL(8)
#define IO_APIC_LOGICAL       BITUL(11)
#define IO_APIC_LEVEL         BITUL(15)
#define IO_APIC_LOW           BITUL(13)
#define IO_APIC_MASK          BITUL(16)

#define LOCAL_APIC_MAX_COUNT  32
#define LOCAL_APIC_ID_REG     0x08U
#define LOCAL_APIC_LD_REG     0x34U
#define LOCAL_APIC_SI_REG     0x3CU
#define LOCAL_APIC_EOI_REG    0x2CU
#define LOCAL_APIC_PAGE_FLAGS APIC_PAGE_FLAGS


#define SPURIOUS_INTERRUPT_VECTOR 0xFFU


struct io_apic {
	void *iospace;
	unsigned gsi;
	unsigned irqs;
	unsigned id;
};

static struct io_apic io_apic[IO_APIC_MAX_COUNT];
static size_t io_apic_size;


struct isa_irq_map {
	struct io_apic *io_apic;
	unsigned long flags;
	unsigned gsi;
	unsigned pin;
};

static struct isa_irq_map isa_irq[ISA_IRQ_COUNT];


struct isa_irq_cfg {
	unsigned long flags;
	unsigned isa;
	unsigned gsi;
};

struct local_apic {
	unsigned pid;
	unsigned lid;
};

static struct local_apic local_apic[LOCAL_APIC_MAX_COUNT];
static void *local_apic_iospace;
static size_t local_apic_count;


static uint32_t io_apic_read(void *vaddr, unsigned reg)
{
	volatile uint32_t *io = vaddr;

	*io = reg & 0xFFU;
	return *(io + 4);
}

static uint32_t local_apic_read(unsigned reg)
{
	volatile uint32_t *io = local_apic_iospace;

	return io[reg];
}

static void io_apic_write(void *vaddr, unsigned reg, uint32_t value)
{
	volatile uint32_t *io = vaddr;

	*io = reg & 0xFFU;
	*(io + 4) = value;
}

static void local_apic_write(unsigned reg, uint32_t value)
{
	volatile uint32_t *io = local_apic_iospace;

	io[reg] = value;
}

static size_t io_apic_irqs(struct io_apic *apic)
{
	const uint32_t version = io_apic_read(apic->iospace, IO_APIC_VER_REG);

	return 1 + ((version >> 16) & 0xFFU);
}

static unsigned io_apic_id(struct io_apic *apic)
{
	return (io_apic_read(apic->iospace, IO_APIC_ID_REG) >> 24) & 0x0FU;
}

static unsigned local_apic_pid(void)
{
	return local_apic_read(LOCAL_APIC_ID_REG) >> 24;
}

static struct io_apic *io_apic_find(unsigned gsi)
{
	for (unsigned i = 0; i != io_apic_size; ++i) {
		struct io_apic *apic = io_apic + i;

		if (apic->gsi <= gsi && apic->gsi + apic->irqs > gsi)
			return apic;
	}
	panic("Cannot find IOAPIC for gsi %u\n", gsi);
	return 0;
}

static void io_apic_setup_isa(const struct isa_irq_cfg *cfg)
{
	assert(cfg->isa < ISA_IRQ_COUNT, "Cannot remap non-ISA interrupts\n");
	struct io_apic *apic = io_apic_find(cfg->gsi);

	isa_irq[cfg->isa].gsi = cfg->gsi;
	isa_irq[cfg->isa].io_apic = apic;
	isa_irq[cfg->isa].pin = cfg->gsi - apic->gsi;
	isa_irq[cfg->isa].flags = cfg->flags;
}

static void io_apic_isa_defaults(void)
{
	for (unsigned i = 0; i != ISA_IRQ_COUNT; ++i) {
		struct isa_irq_cfg cfg = { i, i, HWIRQ_RISING };

		io_apic_setup_isa(&cfg);
	}
}

static void io_apic_add(unsigned long paddr, unsigned gsi)
{
	assert(io_apic_size < IO_APIC_MAX_COUNT, "Too many IOAPICs\n");

	struct io_apic *apic = io_apic + io_apic_size;

	apic->iospace = kmap(paddr, IO_APIC_PAGE_FLAGS);
	apic->gsi = gsi;
	apic->irqs = io_apic_irqs(apic);
	apic->id = io_apic_id(apic);
	++io_apic_size;
}

static void acpi_handle_io_apic(const struct acpi_madt_header *header)
{
	const struct acpi_io_apic *apic = (const void *)header;

	if (io_apic_size == IO_APIC_MAX_COUNT) {
		debug("Ignore IO APIC[%u]\n", apic->apic_id);
		return;
	}

	io_apic_add(apic->apic_paddr, apic->gsi_base);
}

static void acpi_handle_isa_source(const struct acpi_madt_header *header)
{
	const struct acpi_isa_source *source = (const void *)header;
	unsigned long flags = 0;
	unsigned polarity = source->flags & ISA_POLARITY_MASK;
	unsigned trigger = source->flags & ISA_TRIGGER_MASK;
	struct isa_irq_cfg cfg;

	if (source->source == source->gsi && !polarity && !trigger) {
		debug("Useless ISA source override entry\n");
		return;
	}

	memset(&cfg, 0, sizeof(cfg));

	if (!polarity)
		polarity = ISA_POLARITY_HIGH;

	if (!trigger)
		trigger = ISA_TRIGGER_EDGE;

	if (trigger == ISA_TRIGGER_LEVEL)
		flags |= (polarity == ISA_POLARITY_HIGH)
			? HWIRQ_HIGH : HWIRQ_LOW;
	else
		flags |= (polarity == ISA_POLARITY_HIGH)
			? HWIRQ_RISING : HWIRQ_FALLING;

	cfg.isa = source->source;
	cfg.gsi = source->gsi;
	cfg.flags = flags;
	io_apic_setup_isa(&cfg);
}

static void acpi_handle_local_apic(const struct acpi_madt_header *header)
{
	const struct acpi_local_apic *apic = (const void *)header;

	if (local_apic_count == LOCAL_APIC_MAX_COUNT) {
		debug("Too many Local APICs, ignore Local APIC[%u]\n",
			apic->apic_id);
		return;
	}
	debug("Fould Local APIC[%u]\n", apic->apic_id);
	local_apic[local_apic_count++].pid = apic->apic_id;
}

static void setup_io_apic_acpi(void)
{
	acpi_parse_madt(MADT_IO_APIC, &acpi_handle_io_apic);
	io_apic_isa_defaults();
	acpi_parse_madt(MADT_ISA_SOURCE, &acpi_handle_isa_source);
}

static void setup_local_apic_acpi(void)
{
	const struct acpi_madt *tbl =
		(const void *)acpi_table_find(ACPI_MADT_SIGN);

	acpi_parse_madt(MADT_LOCAL_APIC, acpi_handle_local_apic);
	local_apic_iospace = kmap(tbl->lapic_paddr, LOCAL_APIC_PAGE_FLAGS);
}

static void io_apic_setup_irq(struct io_apic *apic, unsigned pin, unsigned ino,
			unsigned long flags, unsigned cpus)
{
	/*
	 * interrupt vector number: [0  - 8)
	 * delivery mode:           [8  - 11)
	 * destination mode:        [11 - 12)
	 * delivery status:         [12 - 13)
	 * pin polarity:            [13 - 14)
	 * IRR (not used):          [14 - 15)
	 * trigger mode:            [15 - 15)
	 * mask/unmask:             [16 - 17)
	 * destination:             [56 - 64)
	 */
	const uint32_t high = (cpus & 0xFFU) << 24;
	uint32_t low = (ino & 0xFFU) | IO_APIC_LOW_PRI | IO_APIC_LOGICAL;

	if (flags & HWIRQ_DISABLED)
		low |= IO_APIC_MASK;

	if (flags & (HWIRQ_FALLING | HWIRQ_LOW))
		low |= IO_APIC_LOW;

	if (flags & (HWIRQ_HIGH | HWIRQ_LOW))
		low |= IO_APIC_LEVEL;

	io_apic_write(apic->iospace, IO_APIC_REDTBL_REG + 2 * pin + 1, high);
	io_apic_write(apic->iospace, IO_APIC_REDTBL_REG + 2 * pin, low);
}

static void apic_setup(unsigned irq, unsigned long flags)
{
	struct io_apic *apic = 0;
	unsigned pin = 0;
	unsigned gsi = irq;

	if (irq < ISA_IRQ_COUNT) {
		if (flags)
			debug("Request ISA irq with flags, flags ignored\n");

		if (flags & HWIRQ_DISABLED)
			flags = isa_irq[irq].flags | HWIRQ_DISABLED;
		else
			flags = isa_irq[irq].flags;

		gsi = isa_irq[irq].gsi;
		apic = isa_irq[irq].io_apic;
		pin = isa_irq[irq].pin;
	}

	if (!apic) {
		apic = io_apic_find(gsi);
		pin = gsi - apic->gsi;
	}

	io_apic_setup_irq(apic, pin, gsi + HW_INTERRUPT_OFFSET, flags,
		IO_APIC_DEST_ALL);
}

static void apic_mask(unsigned irq)
{
	struct io_apic *apic = 0;
	unsigned pin = 0;
	unsigned gsi = irq;

	if (irq < ISA_IRQ_COUNT) {
		gsi = isa_irq[irq].gsi;
		apic = isa_irq[irq].io_apic;
		pin = isa_irq[irq].pin;
	}

	if (!apic) {
		apic = io_apic_find(gsi);
		pin = gsi - apic->gsi;
	}

	const uint32_t low = io_apic_read(apic->iospace,
		IO_APIC_REDTBL_REG + 2 * pin);

	io_apic_write(apic->iospace, IO_APIC_REDTBL_REG + 2 * pin,
		low | IO_APIC_MASK);
}

static void apic_unmask(unsigned irq)
{
	struct io_apic *apic = 0;
	unsigned pin = 0;
	unsigned gsi = irq;

	if (irq < ISA_IRQ_COUNT) {
		gsi = isa_irq[irq].gsi;
		apic = isa_irq[irq].io_apic;
		pin = isa_irq[irq].pin;
	}

	if (!apic) {
		apic = io_apic_find(gsi);
		pin = gsi - apic->gsi;
	}

	const uint32_t low = io_apic_read(apic->iospace,
		IO_APIC_REDTBL_REG + 2 * pin);

	io_apic_write(apic->iospace, IO_APIC_REDTBL_REG + 2 * pin,
		low & ~IO_APIC_MASK);
}

static void apic_eoi(unsigned irq)
{
	(void) irq;
	local_apic_write(LOCAL_APIC_EOI_REG, 0);
}

static const struct irqchip apic = {
	.setup = &apic_setup,
	.mask = &apic_mask,
	.unmask = &apic_unmask,
	.eoi = &apic_eoi
};

void setup_apic(void)
{
	setup_io_apic_acpi();
	setup_local_apic_acpi();
	irqchip_register(&apic);
}

void setup_local_apic(void)
{
	const unsigned pid = local_apic_pid();

	for (unsigned i = 0; i != local_apic_count; ++i) {
		if (pid == local_apic[i].pid) {
			const unsigned id = BITU(i % 8);

			local_apic_write(LOCAL_APIC_LD_REG, id << 24);
			local_apic_write(LOCAL_APIC_SI_REG,
				SPURIOUS_INTERRUPT_VECTOR | BITU(8));
			return;
		}
	}
	panic("Cannot find Local APIC[%u]\n", pid);
}
