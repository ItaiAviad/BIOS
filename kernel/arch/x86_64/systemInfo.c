#include <arch/x86_64/systemInfo.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h> // Make sure to include string.h for memcmp


// ACPI ______________________________________________________________________________________________________________


// Functions to calculate checksum of ACPI structures
static uint8_t acpi_checksum(uint8_t *ptr, uint64_t length) {
    uint8_t sum = 0;
    for (uint64_t i = 0; i < length; ++i) {
        sum += ptr[i];
    }
    return sum;
}

// Locate RSDP by scanning memory (EBDA and first 1MB of memory)
rsdp_descriptor_2 *find_rsdp(void) {
    uint32_t start_address = 0x000E0000;
    uint32_t end_address = 0x000FFFFF;

    for (uint32_t address = start_address; address < end_address; address += 16) {
        rsdp_descriptor_2 *rsdp = (rsdp_descriptor_2 *) (uintptr_t) address;
        if (memcmp(rsdp->signature, RSDP_SIGNATURE, 3) == 0 && (uint8_t)acpi_checksum(rsdp, 20) == 0) {
            return rsdp;
        }
    }
    return NULL;
}

// Function to parse SRAT and calculate total memory
void parse_srat(acpi_sdt_header *srat, uint64_t *total_memory) {
    uint8_t *current = (uint8_t *)srat + sizeof(acpi_sdt_header);
    uint8_t *end = (uint8_t *)srat + srat->length;

    while (current < end) {
        struct srat_memory_affinity *memory_affinity = (struct srat_memory_affinity *)current;

        // Only process memory affinity structures (type 0x01)
        if (memory_affinity->type == 0x01) {
            // Check if the memory region is enabled
            if (memory_affinity->flags & 1) {
                // Sum the length of the enabled memory regions
                *total_memory += memory_affinity->length;

                // Print memory region details (for debugging)
                printf("Base Address: %x, Length: %x bytes\n",
                    memory_affinity->base_address,
                    memory_affinity->length);
            }
        }

        // Move to the next structure in the SRAT
        current += memory_affinity->length;
    }
}

// Function to parse XSDT
void parse_xsdt(rsdp_descriptor_2 *rsdp, uint64_t *total_memory) {
    // Ensure the XSDT address is valid
    if (rsdp->xsdt_address == 0) {
        printf("XSDT address is NULL! Checking for RSDT...\n");
        return; // Proceed to check RSDT in the main function
    }

    xsdt_table *xsdt = (xsdt_table *) (uintptr_t) rsdp->xsdt_address;

    // Check if the XSDT signature is valid
    if (memcmp(xsdt->header.signature, "XSDT", 4) != 0) {
        printf("Problem in XSDT parsing: expected 'XSDT', got %x\n");
        return;
    }

    // Optionally validate the XSDT checksum
    if (acpi_checksum((uint8_t *)xsdt, xsdt->header.length) != 0) {
        printf("XSDT checksum failed!\n");
        return;
    }

    // Iterate through the XSDT entries
    uint64_t entry_count = (xsdt->header.length - sizeof(acpi_sdt_header)) / sizeof(uint64_t);
    for (uint64_t i = 0; i < entry_count; ++i) {
        acpi_sdt_header *entry = (acpi_sdt_header *) (uintptr_t) xsdt->entries[i];

        // Check for SRAT or other memory-related ACPI tables
        if (memcmp(entry->signature, "SRAT", 4) == 0) {
            // Parse SRAT table to find memory regions
            parse_srat(entry, total_memory);
        }
    }
}

// Function to parse RSDT
void parse_radt(rsdp_descriptor_2 *rsdp, uint64_t *total_memory) {
    // Ensure the RSDT address is valid
    if (rsdp->rsdt_address == 0) {
        printf("RSDT address is NULL!\n");
        return;
    }

    struct rsdt_table *rsdt = (struct rsdt_table *) (uint32_t) rsdp->rsdt_address;
    

    // Check if the RSDT signature is valid
    if (memcmp(rsdt->header.signature, "RSDT", 4) != 0) {
        printf("Problem in RSDT parsing'\n");
        return;
    }

    // Iterate through the RSDT entries
    uint32_t entry_count = (rsdt->header.length - sizeof(acpi_sdt_header)) / sizeof(uint32_t);
    for (uint32_t i = 0; i < entry_count; ++i) {
        acpi_sdt_header *entry = (acpi_sdt_header *) (uintptr_t) rsdt->entries[i];

        // Check for SRAT or other memory-related ACPI tables
        if (memcmp(entry->signature, "SRAT", 4) == 0) {
            // Parse SRAT table to find memory regions
            parse_srat(entry, total_memory);
        }
    }
}

// Main function to get memory size
uint64_t get_mem_size_acpi() {
    uint64_t total_memory = 0;

    // Locate the RSDP in memory
    rsdp_descriptor_2 *rsdp = find_rsdp();
    if (!rsdp) {
        printf("RSDP not found!\n");
        return 0;
    }

    // Parse the XSDT or fallback to RSDT
    parse_xsdt(rsdp, &total_memory);

    // If XSDT is NULL, try parsing RSDT
    if (rsdp->xsdt_address == 0) {
        parse_radt(rsdp, &total_memory);
    }

    // Print total available memory
    printf("Total available memory: %llu bytes\n", total_memory);
    return total_memory;
}// __________________________________________________________________


// Function to calculate checksum
static uint8_t smbios_checksum(uint8_t *ptr, uint64_t length) {
    uint8_t sum = 0;
    for (uint64_t i = 0; i < length; ++i) {
        sum += ptr[i];
    }
    return sum;
}

// Function to find SMBIOS entry point
smbios_entry_point* find_smbios_entry_point(void) {
    uint8_t *ptr = (uint8_t *)0xF0000; // Scan from BIOS area
    for (uint32_t i = 0; i < 0xFFFF; i += 16) {
        if (memcmp(ptr + i, "_SM_", 4) == 0) {
            smbios_entry_point *entry = (smbios_entry_point *)(ptr + i);
            // Validate checksum
            if (smbios_checksum((uint8_t *)entry, entry->length) == 0) {
                return entry;
            }
        }
    }
    return NULL;
}

// Function to get total memory size from SMBIOS
uint64_t get_memory_size_from_smbios(void) {
    smbios_entry_point *entry = find_smbios_entry_point();
    if (!entry) {
        printf("SMBIOS entry point not found!\n");
        return 0;
    }

    uint64_t total_memory = 0;
    uint64_t structure_table_address = entry->structure_table_address;

    // Iterate through SMBIOS structures
    for (int i = 0; i < entry->number_of_structures; i++) {
        smbios_memory_device_17 *memory_device = (smbios_memory_device_17 *)(structure_table_address + i * entry->max_structure_size);
        printf("Type: %d", memory_device->type);
        if (memory_device->type == 17) { 
            total_memory += memory_device->size;
            printf("Memory Device %d: Size = %d MB\n", i, memory_device->size);
        }
        if (memory_device->type == 0x6) { 
            smbios_memory_device_6* memory_device_6 = (smbios_memory_device_6 *)memory_device;
            uint64_t mem_size_mb = pow(2, memory_device_6->installed_size);
            total_memory += mem_size_mb;
            printf("Memory Device %d: Size = %d MB\n", i, mem_size_mb);
        }
    }

    printf("Total Memory Size: %d MB\n", total_memory);
    return total_memory;
}
