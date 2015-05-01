#ifndef __KERNEL_ACPI_H__
#define __KERNEL_ACPI_H__

#include <stdint.h>

#define ACPI_RSDP_SIGN_SIZE            8
#define ACPI_OEM_ID_SIZE               6

struct acpi_rsdp {
	char signature[ACPI_RSDP_SIGN_SIZE];
	uint8_t checksum;
	char oem_id[ACPI_OEM_ID_SIZE];
	uint8_t revision;
	uint32_t rsdt_paddr;
	uint32_t size;
	uint64_t xsdt_paddr;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __attribute__((packed));


#define ACPI_OEM_TABLE_ID_SIZE         8
#define ACPI_TABLE_SIGN_SIZE           4

struct acpi_table_header {
	char signature[ACPI_TABLE_SIGN_SIZE];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[ACPI_OEM_ID_SIZE];
	char oem_table_id[ACPI_OEM_TABLE_ID_SIZE];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((packed));

struct acpi_rsdt {
	struct acpi_table_header header;
	uint32_t entries[];
} __attribute__((packed));


#define ACPI_MADT_SIGN                 "APIC"

struct acpi_madt {
	struct acpi_table_header header;
	uint32_t lapic_paddr;
	uint32_t flags;
} __attribute__((packed));

enum acpi_madt_type {
	madt_lapic      = 0x0,
	madt_ioapic     = 0x1,
	madt_isa_source = 0x2,
};

struct acpi_madt_header {
	uint8_t type;
	uint8_t length;
} __attribute__((packed));

struct acpi_isa_source {
	struct acpi_madt_header header;
	uint8_t bus;
	uint8_t source;
	uint8_t gsi;
	uint16_t flags;
} __attribute__((packed));

struct acpi_lapic {
	struct acpi_madt_header header;
	uint8_t cpu_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed));

struct acpi_ioapic {
	struct acpi_madt_header header;
	uint8_t apic_id;
	uint8_t reserved;
	uint32_t apic_paddr;
	uint32_t gsi_base;
} __attribute__((packed));


const struct acpi_rsdp* acpi_rsdp_probe(void);
void setup_acpi(void);
int acpi_available(void);
const struct acpi_table_header *acpi_table_find(const char *sign);

#endif /*__KERNEL_ACPI_H__*/
