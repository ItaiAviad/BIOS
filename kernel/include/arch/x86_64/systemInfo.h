#include <stdint.h>
#include <types.h>
#include <string.h>


// ACPI______________________________________
#define RSDP_SIGNATURE "RSD PTR "
#define EBDA_ADDRESS    0x40E

// RSDP Descriptor Structure (ACPI version 2)
typedef struct {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp_descriptor_2;

// ACPI System Description Table Header
typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header;

// XSDT Structure
typedef struct {
    acpi_sdt_header header;
    uint64_t entries[];
} __attribute__((packed)) xsdt_table;

// SRAT Memory Affinity Structure
struct srat_memory_affinity {
    uint8_t  type;
    uint8_t  length;
    uint32_t proximity_domain;
    uint16_t reserved1;
    uint64_t base_address;
    uint64_t length_2;
    uint32_t reserved2;
    uint32_t flags;
    uint64_t reserved3;
} __attribute__((packed));

struct rsdt_table{
    acpi_sdt_header header;  // ACPI SDT Header
    uint32_t entries[];      // Array of 32-bit physical addresses
} __attribute__((packed));

uint64_t get_mem_size_acpi();

//SMBIOS _____________________________________

// Define the SMBIOS structure
typedef struct __attribute__((packed)) {
    uint8_t  anchor[4];          // Should be "_SM_"
    uint8_t  checksum;           // Checksum of this structure
    uint8_t  length;             // Length of this structure
    uint8_t  major_version;      // Major version number
    uint8_t  minor_version;      // Minor version number
    uint16_t max_structure_size; // Max structure size
    uint8_t  entry_revision;     // Entry point revision
    uint8_t  formatted_area[5];  // Reserved
    uint8_t  intermediate_anchor[5]; // Reserved
    uint8_t  Intermediate_checksum;
    uint16_t structure_table_length; // Length of the structure table
    uint32_t  structure_table_address; // Physical address of the structure table
    uint16_t number_of_structures; // Number of structures
    uint8_t  bcd_revision;       // BCD revision
}  smbios_entry_point;

// Define the structure for SMBIOS type 17 (Memory Device)
typedef struct __attribute__((packed)) {
    uint8_t  type;               // Type of the structure (17 for Memory Device)
    uint8_t  length;             // Length of the structure
    uint16_t handle;             // Handle to this structure
    uint16_t physical_memory_array_handle; // Handle to the memory array
    uint16_t memory_error_info_handle; // Handle to memory error info
    uint8_t  total_width;        // Total width of the memory device
    uint8_t  data_width;         // Data width of the memory device
    uint16_t size;               // Size of the memory device (in MB)
    uint8_t  form_factor;        // Form factor
    uint8_t  device_set;         // Device set
    uint8_t  device_locator;     // Device locator string
    uint8_t  bank_locator;       // Bank locator string
    uint8_t  memory_type;        // Type of memory (e.g. DDR4)
    uint8_t  type_detail;        // Detailed type of memory
    uint16_t speed;              // Speed of memory
    uint8_t  manufacturer;        // Manufacturer string
    uint8_t  serial_number;      // Serial number string
    uint8_t  asset_tag;          // Asset tag string
    uint8_t  part_number;        // Part number string
    uint8_t  attributes;         // Attributes
    uint32_t extended_size;      // Extended size (if applicable)
    uint8_t  extended_memory_type; // Extended memory type
    uint8_t  reserved[3];        // Reserved bytes
} smbios_memory_device_17;

typedef struct __attribute__((packed)) {
    uint8_t  type;               // Type of the structure (17 for Memory Device)
    uint8_t  length;             // Length of the structure
    uint16_t handle;             // Handle to this structure
    uint8_t socket_designation;
    uint8_t bank_connections;
    uint8_t speed;
    uint16_t mem_type;
    uint8_t installed_size;
    uint8_t enabled_size;
    uint8_t error_status;
} smbios_memory_device_6;


uint64_t get_memory_size_from_smbios(void);