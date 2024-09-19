#include "arch/x86_64/io.h"
#include "arch/x86_64/mlayout.h"
#include "arch/x86_64/mmu.h"
#include "dataStructrures/linkedList.h"
#include <pci.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

size_t num_of_used_PCI_bar_pages;
linkedListNode *listPCIDevices;

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

  uint16_t vendorID = getVendorId(bus, device, function);
  if (vendorID == 0xFFFF)
    return; // Device doesn't exist
  check_function(bus, device, function);
  uint8_t headerType = getHeaderType(bus, device, function);
  if ((headerType & 0x80) != 0) {
    // It's a multi-function device, so check remaining functions
    for (function = 1; function < 8; function++) {
      if (getVendorId(bus, device, function) != 0xFFFF) {
        check_function(bus, device, function);
      }
    }
  }
}

void check_bus(uint8_t bus) {
  uint8_t device;

  for (device = 0; device < 32; device++) {
    check_device(bus, device);
  }
}

void check_function(uint8_t bus, uint8_t slot, uint8_t function) {
  uint8_t baseClass;
  uint8_t subClass;
  uint8_t secondaryBus;

  baseClass = getClassCode(bus, slot, function);
  subClass = getSubclass(bus, slot, function);
  if ((baseClass == 0x6) && (subClass == 0x4)) {
    secondaryBus = getSubclass(bus, slot, function);
    check_bus(secondaryBus);
  } else {

    PCIDevice *pciDevice = malloc(sizeof(PCIDevice));
    pciDevice->bus = bus;
    pciDevice->slot = slot;
    pciDevice->function = function;

    pciDevice->vendorId = getVendorId(bus, slot, function);
    pciDevice->deviceId = getProductId(bus, slot, function);

    pciDevice->classCode = getClassCode(bus, slot, function);
    pciDevice->subclass = getSubclass(bus, slot, function);
    pciDevice->progIf = getProgIf(bus, slot, function);

    append_node(&listPCIDevices, (void *)pciDevice);
  }
}

void enumeratePCI() {
  listPCIDevices = (linkedListNode *)NULL;
  uint8_t function;
  uint8_t bus;

  uint8_t headerType = getHeaderType(0, 0, 0);
  if ((headerType & 0x80) == 0) {
    // Single PCI host controller
    check_bus(0);
  } else {
    // Multiple PCI host controllers
    for (function = 0; function < 8; function++) {
      if (getVendorId(0, 0, function) != 0xFFFF)
        break;
      bus = function;
      check_bus(bus);
    }
  }
}

void print_PCI_devices() {
  linkedListNode *head = (linkedListNode *)listPCIDevices;
  printf("__PCI__\n");
  while (head != NULL) {
    PCIDevice *device = (PCIDevice *)head->data;
    printf("%x:%x.%d %x %x %x %x, ", device->bus, device->slot, device->function,
           device->vendorId, device->deviceId, device->classCode, device->subclass);
    head = (linkedListNode *)head->next;
  }
  printf("__PCI_END__\n");
}

void* assign_bar(PCIDevice device, uint8_t bar_num) {
  uint16_t offset = PCI_OFFSET_BASE_ADDRESS_0 + sizeof(uint32_t) * bar_num;
  uint16_t addr = PCI_BAR_START + PAGE_SIZE * num_of_used_PCI_bar_pages;

  uint32_t orig_reg_val = pci_config_read_dword(device.bus, device.slot, device.function, offset);
  pci_config_write_dword(device.bus, device.slot, device.function, offset, 0xFFFFFFFF);

  uint32_t bar_size =
      pci_config_read_dword(device.bus, device.slot, device.function, offset) & 0xFFFFFFF0;
  
  map_memory_range(k_ctx, addr, addr+bar_size-1, addr);

  uint32_t new_bar_value = (addr & 0xFFFFFFF0) | (orig_reg_val & ~0xFFFFFFF0);
  pci_config_write_dword(device.bus, device.slot, device.function, offset, new_bar_value);

  num_of_used_PCI_bar_pages += bar_size/PAGE_SIZE;
  return (void*) addr;
}