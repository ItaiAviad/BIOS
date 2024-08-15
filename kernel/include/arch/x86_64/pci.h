#include <arch/x86_64/io.h>
#include <dataStructrures/linkedList.h>
#include <stdint.h>
#include <types.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

// Offsets for the 0x0 pci header
#define PCI_OFFSET_VENDOR_ID 0x00
#define PCI_OFFSET_DEVICE_ID 0x02
#define PCI_OFFSET_COMMAND 0x04
#define PCI_OFFSET_STATUS 0x06
#define PCI_OFFSET_REVISION_ID 0x08
#define PCI_OFFSET_PROG_IF 0x09
#define PCI_OFFSET_SUBCLASS 0x0A
#define PCI_OFFSET_CLASS_CODE 0x0B
#define PCI_OFFSET_CACHE_LINE_SIZE 0x0C
#define PCI_OFFSET_LATENCY_TIMER 0x0D
#define PCI_OFFSET_HEADER_TYPE 0x0E
#define PCI_OFFSET_BIST 0x0F
#define PCI_OFFSET_BASE_ADDRESS_0 0x10
#define PCI_OFFSET_BASE_ADDRESS_1 0x14
#define PCI_OFFSET_BASE_ADDRESS_2 0x18
#define PCI_OFFSET_BASE_ADDRESS_3 0x1C
#define PCI_OFFSET_BASE_ADDRESS_4 0x20
#define PCI_OFFSET_BASE_ADDRESS_5 0x24
#define PCI_OFFSET_CARD_BUS_CIS_POINTER 0x28
#define PCI_OFFSET_SUBSYSTEM_VENDOR_ID 0x2C
#define PCI_OFFSET_SUBSYSTEM_ID 0x2E
#define PCI_OFFSET_EXPANSION_ROM_BASE_ADDRESS 0x30
#define PCI_OFFSET_CAPABILITY_POINTER 0x34
#define PCI_OFFSET_INTERRUPT_LINE 0x3C
#define PCI_OFFSET_INTERRUPT_PIN 0x3D
#define PCI_OFFSET_MIN_GRANT 0x3E
#define PCI_OFFSET_MAX_LATENCY 0x3F

extern linkedListNode *listPCIDevices;

typedef struct PCIDevice {
  //  Device localization data:
  uint8_t bus;
  uint8_t slot;

  //  Device Identity data:
  uint8_t vendorId;
  uint8_t deviceId;

  // Device type data
  uint8_t classCode;
  uint8_t subclass;
  uint8_t progIf;

} PCIDevice;

/**
 * @brief Read from a word at an offset from a pci device config.
 * @param bus
 * @param slot
 * @param func
 * @param offset
 */
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset);

/**
 * @brief Check if a device at a certain bus and slot is a valid pcie device.
 * @param bus
 * @param slot
 */
uint8_t pciCheckDevice(uint8_t bus, uint8_t slot);

/**
 * @brief Go through all available pci devices and add them to the
 * listPCIDevices list.
 */
void initPCIDevList();

inline uint16_t getVendorId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_VENDOR_ID);
}

inline uint16_t getProductId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_DEVICE_ID);
}

inline uint16_t getCommand(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_COMMAND);
}

inline uint16_t getStatus(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_STATUS);
}

inline uint8_t getRevisionId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_REVISION_ID) & 0xFF;
}

inline uint8_t getProgIf(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_PROG_IF) & 0xFF;
}

inline uint8_t getSubclass(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_SUBCLASS) & 0xFF;
}

inline uint8_t getClassCode(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_CLASS_CODE) & 0xFF;
}
inline uint8_t getCacheLineSize(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_CACHE_LINE_SIZE) & 0xFF;
}

inline uint8_t getLatencyTimer(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_LATENCY_TIMER) & 0xFF;
}

inline uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_HEADER_TYPE) & 0xFF;
}

inline uint8_t getBIST(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_BIST) & 0xFF;
}

inline uint32_t getBar0(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_0;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint32_t getBar1(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_1;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint32_t getBar2(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_2;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint32_t getBar3(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_3;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint32_t getBar4(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_4;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint32_t getBar5(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_5;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint32_t getCardbusCISPointer(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_CARD_BUS_CIS_POINTER;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint16_t getSubsystemVendorId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_SUBSYSTEM_VENDOR_ID);
}

inline uint16_t getSubsystemId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_SUBSYSTEM_ID);
}

inline uint32_t getExpensionRomBaseAddr(uint8_t bus, uint8_t slot,
                                        uint8_t func) {
  uint8_t offset = PCI_OFFSET_EXPANSION_ROM_BASE_ADDRESS;
  return pciConfigReadWord(bus, slot, func, offset) +
         ((uint32_t)pciConfigReadWord(bus, slot, func, offset + 2) << 0x10);
}

inline uint8_t getCapabilitiesPointer(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_CAPABILITY_POINTER) &
         0xFF;
}

inline uint8_t getInterruptLine(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_INTERRUPT_LINE) & 0xFF;
}

inline uint8_t getInterruptPin(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_INTERRUPT_PIN) & 0xFF;
}

inline uint8_t getMinGrant(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_MIN_GRANT) & 0xFF;
}

inline uint8_t getMaxLatency(uint8_t bus, uint8_t slot, uint8_t func) {
  return pciConfigReadWord(bus, slot, func, PCI_OFFSET_MAX_LATENCY) & 0xFF;
}
