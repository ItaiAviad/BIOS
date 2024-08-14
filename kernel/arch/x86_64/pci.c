#include <arch/x86_64/pci.h>

linkedListNode* listPCIDevices;

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
  
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outl(PCI_CONFIG_ADDRESS, address);
    
    tmp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

uint8_t pciCheckDevice(uint8_t bus, uint8_t slot){
    return (pciConfigReadWord(bus, slot, 0, PCI_OFFSET_VENDOR) != 0xffff);
}