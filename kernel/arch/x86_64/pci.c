#include <arch/x86_64/pci.h>
#include <stdint.h>

linkedListNode *listPCIDevices;

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
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

void checkDevice(uint8_t bus, uint8_t device) {
  uint8_t function = 0;

  uint16_t vendorID = getVendorId(bus, device, function);
  if (vendorID == 0xFFFF)
    return; // Device doesn't exist
  checkFunction(bus, device, function);
  uint8_t headerType = getHeaderType(bus, device, function);
  if ((headerType & 0x80) != 0) {
    // It's a multi-function device, so check remaining functions
    for (function = 1; function < 8; function++) {
      if (getVendorId(bus, device, function) != 0xFFFF) {
        checkFunction(bus, device, function);
      }
    }
  }
}

void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
  uint8_t baseClass;
  uint8_t subClass;
  uint8_t secondaryBus;

  baseClass = getClassCode(bus, device, function);
  subClass = getSubclass(bus, device, function);
  if ((baseClass == 0x6) && (subClass == 0x4)) {
    secondaryBus = getSubclass(bus, device, function);
    checkBus(secondaryBus);
  }
}

void initPCIDevList() {}
