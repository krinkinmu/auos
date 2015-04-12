#ifndef __KERNEL_ACPI_H__
#define __KERNEL_ACPI_H__

#include <stdint.h>
#include <stddef.h>

#define ACPI_RSDP_SIGN_SIZE    8
#define ACPI_OEM_TABLE_ID_SIZE 8
#define ACPI_TABLE_SIGN_SIZE   4
#define ACPI_OEM_ID_SIZE       6

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

const struct acpi_table_rsdp *acpi_find_rsdp_table(void);
const struct acpi_table_rsdt *acpi_get_rsdt(const struct acpi_table_rsdp *rsdp);
size_t acpi_table_headers_count(const struct acpi_table_rsdt *rsdt);
const struct acpi_table_header *acpi_table_header_get(
			const struct acpi_table_rsdt *rsdt, size_t idx);

#endif /*__KERNEL_ACPI_H__*/
