#include <kernel/utility.h>
#include <kernel/acpi.h>
#include <arch/memory.h>

static const void *acpi_find_rsdp_sign(const void *from, const void *to)
{
	static const char RSDP_SIGN[] = "RSD PTR ";

	const char *l = (const char *)from;
	const char *r = (const char *)to;

	for (const char *ptr = l; ptr < r; ptr += 16)
		if (!memcmp(RSDP_SIGN, ptr, sizeof(RSDP_SIGN) - 1))
			return ptr;
	return 0;
}

static const void *acpi_find_rsdp_addr(void)
{
	const unsigned long ebda_begin = (unsigned long)
		*((const uint16_t *)virt_addr(0x40EUL << 4));
	const unsigned long ebda_end = ebda_begin + 0x400;

	const void *addr = acpi_find_rsdp_sign(virt_addr(ebda_begin),
		virt_addr(ebda_end));

	if (addr)
		return addr;

	return acpi_find_rsdp_sign(virt_addr(0x0E0000UL),
		virt_addr(0x100000UL));
}

const struct acpi_rsdp* acpi_rsdp_probe(void)
{
	const void *addr = acpi_find_rsdp_addr();

	if (addr)
		return (struct acpi_rsdp *)addr;
	return 0;
}
