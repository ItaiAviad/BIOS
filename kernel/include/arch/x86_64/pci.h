#include <types.h>
#include <arch/x86_64/io.h>
#include <dataStructrures/linkedList.h>

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

extern linkedListNode* listPCIDevices;

typedef struct PCIDevice{
//  Device Identity:
    uint8_t bus;
    uint8_t slot;

//  Device data:
    uint8_t vendorId;
    uint8_t deviceId;

} PCIDevice;

/**
    * @brief Read from a word at an offset from a pci device config.
    * @param bus
    * @param slot
    * @param func
    * @param offset
*/
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

/**
    * @brief Check if a device at a certain bus and slot is a valid pcie device.
    * @param bus
    * @param slot
*/
uint8_t pciCheckDevice(uint8_t bus, uint8_t slot);

/**
    * @brief Go through all available pci devices and add them to the listPCIDevices list.
*/
void initPCIDevList();