#ifndef __KERNEL_ACPI_H__
#define __KERNEL_ACPI_H__

#include <stdint.h>
#include <stddef.h>

#define ACPI_RSDP_SIGN_SIZE    8
#define ACPI_OEM_TABLE_ID_SIZE 8
#define ACPI_TABLE_SIGN_SIZE   4
#define ACPI_OEM_ID_SIZE       6

#define ACPI_MADT_SIGN         "APIC"

struct acpi_table_rsdp {
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

struct acpi_table_rsdt {
	struct acpi_table_header header;
	uint32_t entries[];
} __attribute__((packed));

#define MADT_LOCAL_APIC                0x0
#define MADT_IO_APIC                   0x1
#define MADT_IRQ_SRC_OVERRIDE          0x2
#define MADT_IO_APIC_NMI               0x3
#define MADT_LOCAL_APIC_NMI            0x4
#define MADT_LOCAL_APIC_ADDR_OVERRIDE  0x5
#define MADT_IO_SAPIC                  0x6
#define MADT_LOCAL_SAPIC               0x7
#define MADT_PLATFORM_INT_SRC          0x8
#define MADT_LOCAL_X2APIC              0x9
#define MADT_LOCAL_X2APIC_NMI          0xA
#define MADT_LOCAL_GIC                 0xB
#define MADT_GIC_DISTRIB               0xC

struct acpi_irq_header {
	uint8_t type;
	uint8_t length;
} __attribute__((packed));

struct acpi_irq_source_override {
	struct acpi_irq_header header;
	uint8_t bus;
	uint8_t source;
	uint8_t gsi;
	uint16_t flags;
} __attribute__((packed));

struct acpi_local_apic {
	struct acpi_irq_header header;
	uint8_t cpu_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed));

struct acpi_io_apic {
	struct acpi_irq_header header;
	uint8_t apic_id;
	uint8_t reserved;
	uint32_t apic_paddr;
	uint32_t gsi_base;
} __attribute__((packed));

struct acpi_madt_table {
	struct acpi_table_header header;
	uint32_t lapic_paddr;
	uint32_t flags;
	// struct acpi_irq_header headers[];
} __attribute__((packed));

void setup_acpi(void);
int acpi_available(void);
const struct acpi_table_header *acpi_table_find(const char *sign);

#endif /*__KERNEL_ACPI_H__*/
