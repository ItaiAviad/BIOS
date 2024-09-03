#ifndef PCI_H

#define PCI_H

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
  uint8_t function;

  //  Device Identity data:
  uint16_t vendorId;
  uint16_t deviceId;

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
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset);

/**
 * @brief Write a word at an offset to a pci device config.
 * @param bus
 * @param slot
 * @param func
 * @param offset
 * @param value
 */
void pci_config_write_word(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset, uint16_t value);


/**
 * @brief Read from a double-word at an offset from a pci device config.
 * @param bus
 * @param slot
 * @param func
 * @param offset
 */
uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset);

/**
 * @brief Write a double-word at an offset to a pci device config.
 * @param bus
 * @param slot
 * @param func
 * @param offset
 * @param value
 */
void pci_config_write_dword(uint8_t bus, uint8_t slot, uint8_t func,
                           uint8_t offset, uint32_t value);

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
void enumeratePCI();

void print_PCI_devices();

void assign_bar(PCIDevice device, uint8_t bar_num);

void check_function(uint8_t bus, uint8_t device, uint8_t function);

void check_bus(uint8_t bus);

void check_device(uint8_t bus, uint8_t device);

static inline uint16_t getVendorId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_VENDOR_ID);
}

static inline uint16_t getProductId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_DEVICE_ID);
}

static inline uint16_t getCommand(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_COMMAND);
}

static inline uint16_t getStatus(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_STATUS);
}

static inline uint8_t getRevisionId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_REVISION_ID) & 0xFF;
}

static inline uint8_t getProgIf(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_PROG_IF) & 0xFF;
}

static inline uint8_t getSubclass(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_SUBCLASS) & 0xFF;
}

static inline uint8_t getClassCode(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_CLASS_CODE) & 0xFF;
}
static inline uint8_t getCacheLineSize(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_CACHE_LINE_SIZE) & 0xFF;
}

static inline uint8_t getLatencyTimer(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_LATENCY_TIMER) & 0xFF;
}

static inline uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_HEADER_TYPE) & 0xFF;
}

static inline uint8_t getBIST(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_BIST) & 0xFF;
}

static inline uint32_t getBar0(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_0;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint32_t getBar1(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_1;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint32_t getBar2(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_2;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint32_t getBar3(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_3;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint32_t getBar4(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_4;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint32_t getBar5(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_BASE_ADDRESS_5;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint32_t getCardbusCISPointer(uint8_t bus, uint8_t slot, uint8_t func) {
  uint8_t offset = PCI_OFFSET_CARD_BUS_CIS_POINTER;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint16_t getSubsystemVendorId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_SUBSYSTEM_VENDOR_ID);
}

static inline uint16_t getSubsystemId(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_SUBSYSTEM_ID);
}

static inline uint32_t getExpensionRomBaseAddr(uint8_t bus, uint8_t slot,
                                        uint8_t func) {
  uint8_t offset = PCI_OFFSET_EXPANSION_ROM_BASE_ADDRESS;
  return pci_config_read_dword(bus, slot, func, offset);
}

static inline uint8_t getCapabilitiesPointer(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_CAPABILITY_POINTER) &
         0xFF;
}

static inline uint8_t getInterruptLine(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_INTERRUPT_LINE) & 0xFF;
}

static inline uint8_t getInterruptPin(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_INTERRUPT_PIN) & 0xFF;
}

static inline uint8_t getMinGrant(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_MIN_GRANT) & 0xFF;
}

static inline uint8_t getMaxLatency(uint8_t bus, uint8_t slot, uint8_t func) {
  return pci_config_read_word(bus, slot, func, PCI_OFFSET_MAX_LATENCY) & 0xFF;
}

#endif