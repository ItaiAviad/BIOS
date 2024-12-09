#include "arch/x86_64/io.h"
#include "arch/x86_64/mlayout.h"
#include "arch/x86_64/mmu.h"
#include <arch/x86_64/hardwareMem.h>
#include <memory.h>
#include <pci.h>

#include <stdio.h>
#include <stdlib.h>

linkedListNode *list_pci_devices = NULL;

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func,
                              uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(PCI_CONFIG_ADDRESS, address);

    tmp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

void pci_config_write_word(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset, uint16_t value) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    // Calculate the address to write
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write the address to the PCI configuration address register
    outl(PCI_CONFIG_ADDRESS, address);

    // Write the value to the PCI configuration data register
    uint32_t tmp = inl(PCI_CONFIG_DATA);    // Read current value
    tmp &= ~(0xFFFF << ((offset & 2) * 8)); // Clear the bits to be written
    tmp |= (value << ((offset & 2) * 8));   // Set the new value
    outl(PCI_CONFIG_DATA, tmp);             // Write updated value back
}

uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func,
                               uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(PCI_CONFIG_ADDRESS, address);

    tmp = inl(PCI_CONFIG_DATA);
    return tmp;
}

void pci_config_write_dword(uint8_t bus, uint8_t slot, uint8_t func,
                            uint8_t offset, uint32_t value) {
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    // Calculate the address to write
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write the address to the PCI configuration address register
    outl(PCI_CONFIG_ADDRESS, address);

    outl(PCI_CONFIG_DATA, value);
}

void check_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    uint16_t vendorID = get_vendor_id(bus, device, function);
    if (vendorID == 0xFFFF)
        return; // Device doesn't exist
    check_function(bus, device, function);
    uint8_t headerType = get_header_type(bus, device, function);
    if ((headerType & 0x80) != 0) {
        // It's a multi-function device, so check remaining functions
        for (function = 1; function < 8; function++) {
            if (get_vendor_id(bus, device, function) != 0xFFFF) {
                check_function(bus, device, function);
            }
        }
    }
}

void check_bus(uint8_t bus) {
    uint8_t device = {0};

    for (device = 0; device < 32; device++) {
        check_device(bus, device);
    }
}

void check_function(uint8_t bus, uint8_t slot, uint8_t function) {
    uint8_t base_class = 0;
    uint8_t sub_class = 0;
    uint8_t secondary_bus = 0;

    base_class = get_class_code(bus, slot, function);
    sub_class = get_subclass(bus, slot, function);
    if ((base_class == 0x6) && (sub_class == 0x4)) {
        secondary_bus = get_subclass(bus, slot, function);
        check_bus(secondary_bus);
    } else {

        PCIDevice *pciDevice = hardware_allocate_mem(sizeof(PCIDevice), 0);
        pciDevice->bus = bus;
        pciDevice->slot = slot;
        pciDevice->function = function;

        pciDevice->vendorId = get_vendor_id(bus, slot, function);
        pciDevice->deviceId = get_product_id(bus, slot, function);

        pciDevice->classCode = get_class_code(bus, slot, function);
        pciDevice->subclass = get_subclass(bus, slot, function);
        pciDevice->progIf = get_prog_if(bus, slot, function);

        append_node(&list_pci_devices, (void *)pciDevice);
    }
}

void enumerate_pci() {
    list_pci_devices = (linkedListNode *)NULL;
    uint8_t function = 0;
    uint8_t bus = 0;

    uint8_t headerType = get_header_type(0, 0, 0);
    if ((headerType & 0x80) == 0) {
        // Single PCI host controller
        check_bus(0);
    } else {
        // Multiple PCI host controllers
        for (function = 0; function < 8; function++) {
            if (get_vendor_id(0, 0, function) != 0xFFFF)
                break;
            bus = function;
            check_bus(bus);
        }
    }
}

void print_pci_devices() {
    linkedListNode *head = (linkedListNode *)list_pci_devices;
    #ifdef DEBUG
    printf("__PCI__\n");
    #endif
    while (head != NULL) {
        #ifdef DEBUG
        PCIDevice *device = (PCIDevice *)head->data;
        printf("%x:%x.%x %x %x %x %x, ", device->bus, device->slot, device->function,
               device->vendorId, device->deviceId, device->classCode, device->subclass);
        #endif
        head = (linkedListNode *)head->next;
    }
    #ifdef DEBUG
    printf("__PCI_END__\n");
    #endif
}

void *assign_bar(PCIDevice device, uint8_t bar_num) {
    uint16_t config_space_offset = PCI_OFFSET_BASE_ADDRESS_0 + sizeof(uint32_t) * bar_num;

    uint32_t orig_reg_val = pci_config_read_dword(device.bus, device.slot, device.function, config_space_offset);
    pci_config_write_dword(device.bus, device.slot, device.function, config_space_offset, 0xFFFFFFFF);

    uint32_t bar_size =
        pci_config_read_dword(device.bus, device.slot, device.function, config_space_offset) & 0xFFFFFFF0;
    bar_size = ~bar_size;
    bar_size += 1;

    if (bar_size == 0) {
        return NULL;
    }

    map_memory_range_with_flags(kpcb.ctx, (void*) (uint64_t) orig_reg_val, (void*) (uint64_t) orig_reg_val + bar_size - 1, (void*) (uint64_t) orig_reg_val, PAGE_PRESENT | PAGE_WRITE | PAGE_UNCACHEABLE | PAGE_USER, 0);

    flush_tlb();

    pci_config_write_dword(device.bus, device.slot, device.function, config_space_offset, orig_reg_val);
    
    return (void *)(uint64_t) orig_reg_val;
}