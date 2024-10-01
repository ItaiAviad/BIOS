#ifndef AHCI_H

#define AHCI_H

#include <drive.h>
#include <dataStructrures/linkedList.h>
#include <pci.h>

#define AHCI_CONFIG_BAR_NUM 5

#define SATA_SIG_ATA 0x00000101   // SATA drive
#define SATA_SIG_ATAPI 0xEB140101 // SATAPI drive
#define SATA_SIG_SEMB 0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM 0x96690101    // Port multiplier

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define HBA_PxIS_TFES (1 << 30)

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_IDENTIFY 0xEC

#define PRDT_READ_SIZE_PER_ENTRY (16*SECTOR_SIZE) // Must be dividable by the size of a sector
#define PRDT_WRITE_SIZE_PER_ENTRY (16*SECTOR_SIZE) // Must be dividable by the size of a sector

extern linkedListNode *ahci_devices;

void setup_ahci_controllers();

//________________fis_________
typedef enum {
  FIS_TYPE_REG_H2D = 0x27,   // Register FIS - host to device
  FIS_TYPE_REG_D2H = 0x34,   // Register FIS - device to host
  FIS_TYPE_DMA_ACT = 0x39,   // DMA activate FIS - device to host
  FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS - bidirectional
  FIS_TYPE_DATA = 0x46,      // Data FIS - bidirectional
  FIS_TYPE_BIST = 0x58,      // BIST activate FIS - bidirectional
  FIS_TYPE_PIO_SETUP = 0x5F, // PIO setup FIS - device to host
  FIS_TYPE_DEV_BITS = 0xA1,  // Set device bits FIS - device to host
} FIS_TYPE;

typedef volatile struct tagFIS_SET_DEVICE_BITS {
  uint8_t fis_type;
  uint8_t pmport : 4;
  uint8_t rsvd : 2;
  uint8_t i : 1;
  uint8_t n : 1;
  uint8_t statusl : 3;
  uint8_t rsvd2 : 1;
  uint8_t statush : 3;
  uint8_t rsvd3 : 1;
  uint8_t error;
} FIS_DEV_BITS;

typedef struct tagFIS_DMA_SETUP {
  // DWORD 0
  uint8_t fis_type; // FIS_TYPE_DMA_SETUP

  uint8_t pmport : 4; // Port multiplier
  uint8_t rsv0 : 1;   // Reserved
  uint8_t d : 1;      // Data transfer direction, 1 - device to host
  uint8_t i : 1;      // Interrupt bit
  uint8_t a : 1;      // Auto-activate. Specifies if DMA Activate FIS is needed

  uint8_t rsved[2]; // Reserved

  // DWORD 1&2

  uint64_t DMAbufferID; // DMA Buffer Identifier. Used to Identify DMA buffer in
                        // host memory. SATA Spec says host specific and not in
                        // Spec. Trying AHCI spec might work.

  // DWORD 3
  uint32_t rsvd; // More reserved

  // DWORD 4
  uint32_t DMAbufOffset; // Byte offset into buffer. First 2 bits must be 0

  // DWORD 5
  uint32_t TransferCount; // Number of bytes to transfer. Bit 0 must be 0

  // DWORD 6
  uint32_t resvd; // Reserved

} FIS_DMA_SETUP;

typedef struct tagHBA_PRDT_ENTRY {
  uint64_t dba;  // Data base address
  uint32_t rsv0; // Reserved

  // DW3
  uint32_t dbc : 22; // Byte count, 4M max
  uint32_t rsv1 : 9; // Reserved
  uint32_t i : 1;    // Interrupt on completion
} HBA_PRDT_ENTRY;

typedef struct tagHBA_CMD_TBL {
  // 0x00
  uint8_t cfis[64]; // Command FIS

  // 0x40
  uint8_t acmd[16]; // ATAPI command, 12 or 16 bytes

  // 0x50
  uint8_t rsv[48]; // Reserved

  // 0x80
  HBA_PRDT_ENTRY
      prdt_entry[1]; // Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;

typedef struct tagFIS_PIO_SETUP {
  // DWORD 0
  uint8_t fis_type; // FIS_TYPE_PIO_SETUP

  uint8_t pmport : 4; // Port multiplier
  uint8_t rsv0 : 1;   // Reserved
  uint8_t d : 1;      // Data transfer direction, 1 - device to host
  uint8_t i : 1;      // Interrupt bit
  uint8_t rsv1 : 1;

  uint8_t status; // Status register
  uint8_t error;  // Error register

  // DWORD 1
  uint8_t lba0;   // LBA low register, 7:0
  uint8_t lba1;   // LBA mid register, 15:8
  uint8_t lba2;   // LBA high register, 23:16
  uint8_t device; // Device register

  // DWORD 2
  uint8_t lba3; // LBA register, 31:24
  uint8_t lba4; // LBA register, 39:32
  uint8_t lba5; // LBA register, 47:40
  uint8_t rsv2; // Reserved

  // DWORD 3
  uint8_t countl;   // Count register, 7:0
  uint8_t counth;   // Count register, 15:8
  uint8_t rsv3;     // Reserved
  uint8_t e_status; // New value of status register

  // DWORD 4
  uint16_t tc;     // Transfer count
  uint8_t rsv4[2]; // Reserved
} FIS_PIO_SETUP;

typedef struct tagFIS_REG_D2H {
  // DWORD 0
  uint8_t fis_type; // FIS_TYPE_REG_D2H

  uint8_t pmport : 4; // Port multiplier
  uint8_t rsv0 : 2;   // Reserved
  uint8_t i : 1;      // Interrupt bit
  uint8_t rsv1 : 1;   // Reserved

  uint8_t status; // Status register
  uint8_t error;  // Error register

  // DWORD 1
  uint8_t lba0;   // LBA low register, 7:0
  uint8_t lba1;   // LBA mid register, 15:8
  uint8_t lba2;   // LBA high register, 23:16
  uint8_t device; // Device register

  // DWORD 2
  uint8_t lba3; // LBA register, 31:24
  uint8_t lba4; // LBA register, 39:32
  uint8_t lba5; // LBA register, 47:40
  uint8_t rsv2; // Reserved

  // DWORD 3
  uint8_t countl;  // Count register, 7:0
  uint8_t counth;  // Count register, 15:8
  uint8_t rsv3[2]; // Reserved

  // DWORD 4
  uint8_t rsv4[4]; // Reserved
} FIS_REG_D2H;

typedef struct tagFIS_REG_H2D {
  // DWORD 0
  uint8_t fis_type; // FIS_TYPE_REG_H2D

  uint8_t pmport : 4; // Port multiplier
  uint8_t rsv0 : 3;   // Reserved
  uint8_t c : 1;      // 1: Command, 0: Control

  uint8_t command;  // Command register
  uint8_t featurel; // Feature register, 7:0

  // DWORD 1
  uint8_t lba0;   // LBA low register, 7:0
  uint8_t lba1;   // LBA mid register, 15:8
  uint8_t lba2;   // LBA high register, 23:16
  uint8_t device; // Device register

  // DWORD 2
  uint8_t lba3;     // LBA register, 31:24
  uint8_t lba4;     // LBA register, 39:32
  uint8_t lba5;     // LBA register, 47:40
  uint8_t featureh; // Feature register, 15:8

  // DWORD 3
  uint16_t count;  // Count register, 7:0
  uint8_t icc;     // Isochronous command completion
  uint8_t control; // Control register

  // DWORD 4
  uint8_t rsv1[4]; // Reserved
} FIS_REG_H2D;

//_________________HBA_______________

typedef volatile struct tagHBA_PORT {
  uint64_t clb;       // 0x00, command list base address, 1K-byte aligned
  uint64_t fb;        // 0x08, FIS base address, 256-byte aligned
  uint32_t is;        // 0x10, interrupt status
  uint32_t ie;        // 0x14, interrupt enable
  uint32_t cmd;       // 0x18, command and status
  uint32_t rsv0;      // 0x1C, Reserved
  uint32_t tfd;       // 0x20, task file data
  uint32_t sig;       // 0x24, signature
  uint32_t ssts;      // 0x28, SATA status (SCR0:SStatus)
  uint32_t sctl;      // 0x2C, SATA control (SCR2:SControl)
  uint32_t serr;      // 0x30, SATA error (SCR1:SError)
  uint32_t sact;      // 0x34, SATA active (SCR3:SActive)
  uint32_t ci;        // 0x38, command issue
  uint32_t sntf;      // 0x3C, SATA notification (SCR4:SNotification)
  uint32_t fbs;       // 0x40, FIS-based switch control
  uint32_t rsv1[11];  // 0x44 ~ 0x6F, Reserved
  uint32_t vendor[4]; // 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef volatile struct tagHBA_MEM {
  // 0x00 - 0x2B, Generic Host Control
  uint32_t cap;     // 0x00, Host capability
  uint32_t ghc;     // 0x04, Global host control
  uint32_t is;      // 0x08, Interrupt status
  uint32_t pi;      // 0x0C, Port implemented
  uint32_t vs;      // 0x10, Version
  uint32_t ccc_ctl; // 0x14, Command completion coalescing control
  uint32_t ccc_pts; // 0x18, Command completion coalescing ports
  uint32_t em_loc;  // 0x1C, Enclosure management location
  uint32_t em_ctl;  // 0x20, Enclosure management control
  uint32_t cap2;    // 0x24, Host capabilities extended
  uint32_t bohc;    // 0x28, BIOS/OS handoff control and status

  // 0x2C - 0x9F, Reserved
  uint8_t rsv[0xA0 - 0x2C];

  // 0xA0 - 0xFF, Vendor specific registers
  uint8_t vendor[0x100 - 0xA0];

  // 0x100 - 0x10FF, Port control registers
  HBA_PORT ports[32]; // 1 ~ 32
} HBA_MEM;

typedef volatile struct tagHBA_FIS {
  // 0x00
  FIS_DMA_SETUP dsfis; // DMA Setup FIS
  uint8_t pad0[4];

  // 0x20
  FIS_PIO_SETUP psfis; // PIO Setup FIS
  uint8_t pad1[12];

  // 0x40
  FIS_REG_D2H rfis; // Register – Device to Host FIS
  uint8_t pad2[4];

  // 0x58
  FIS_DEV_BITS sdbfis; // Set Device Bit FIS

  // 0x60
  uint8_t ufis[64];

  // 0xA0
  uint8_t rsv[0x100 - 0xA0];
} HBA_FIS;

typedef struct tagHBA_CMD_HEADER {
  // DW0
  uint8_t cfl : 5; // Command FIS length in DWORDS, 2 ~ 16
  uint8_t a : 1;   // ATAPI
  uint8_t w : 1;   // Write, 1: H2D, 0: D2H
  uint8_t p : 1;   // Prefetchable

  uint8_t r : 1;    // Reset
  uint8_t b : 1;    // BIST
  uint8_t c : 1;    // Clear busy upon R_OK
  uint8_t rsv0 : 1; // Reserved
  uint8_t pmp : 4;  // Port multiplier port

  uint16_t prdtl; // Physical region descriptor table length in entries

  // DW1
  volatile uint32_t prdbc; // Physical region descriptor byte count transferred

  // DW2, 3
  uint64_t ctba; // Command table descriptor base address

  // DW4 - 7
  uint32_t rsv1[4]; // Reserved
} HBA_CMD_HEADER;


typedef struct {
    uint16_t  general_config;        // 0: General configuration
    uint16_t  num_cylinders;         // 1: Number of logical cylinders
    uint16_t  reserved_2;            // 2: Reserved
    uint16_t  num_heads;             // 3: Number of logical heads
    uint16_t  reserved_4;            // 4: Reserved
    uint16_t  num_sectors_per_track; // 5: Number of sectors per track
    uint16_t  vendor_specific_6[3];  // 6-8: Vendor specific data
    uint16_t  serial_number[10];     // 10-19: Serial number
    uint16_t  reserved_20;           // 20: Reserved
    uint16_t  firmware_revision[4];  // 23-26: Firmware revision
    uint16_t  model_number[20];      // 27-46: Model number
    uint16_t  reserved_47;           // 47: Reserved
    uint16_t  capabilities_49;       // 49: Capabilities
    uint16_t  capabilities_50;       // 50: Capabilities continued
    uint16_t  field_validity;        // 53: Field validity
    uint16_t  num_current_cylinders; // 54: Number of current cylinders
    uint16_t  num_current_heads;     // 55: Number of current heads
    uint16_t  num_current_sectors;   // 56: Number of current sectors
    uint32_t  current_capacity;   // 57-58: Current total capacity in sectors
    uint16_t  reserved_59;           // 59: Reserved
    uint32_t  user_addressable_sectors; // 60-61: Total user-addressable sectors
    uint16_t  reserved_62;           // 62: Reserved
    uint16_t  multi_sector_settings; // 63: Multi-sector settings
    uint16_t  pio_mode;              // 64: PIO modes supported
    uint16_t  min_multiword_dma_cycle_time;   // 65: Minimum multiword DMA cycle time per word
    uint16_t  recommended_multiword_dma_cycle_time; // 66: Recommended multiword DMA cycle time per word
    uint16_t  min_pio_cycle_time_without_flow_control; // 67: Minimum PIO cycle time without flow control
    uint16_t  min_pio_cycle_time_with_iordy_flow_control; // 68: Minimum PIO cycle time with IORDY flow control
    uint16_t  reserved_69_79[11];    // 69-79: Reserved
    uint16_t  major_version;         // 80: Major version number
    uint16_t  minor_version;         // 81: Minor version number
    uint16_t  command_set_82;        // 82: Command set supported
    uint16_t  command_set_83;        // 83: Command set supported
    uint16_t  command_set_84;        // 84: Command set/feature supported
    uint16_t  command_set_enabled_85; // 85: Command set/feature enabled
    uint16_t  command_set_enabled_86; // 86: Command set/feature enabled
    uint16_t  command_set_enabled_87; // 87: Command set/feature enabled
    uint16_t  ultra_dma_mode;        // 88: Ultra DMA mode
    uint16_t  reserved_89_127[39];   // 89-127: Reserved
    uint16_t  security_status;       // 128: Security status
    uint16_t  reserved_129_159[31];  // 129-159: Reserved
    uint16_t  cfa_power_mode;        // 160: CFA power mode
    uint16_t  reserved_161_255[95];  // 161-255: Reserved
} ATA_IDENTIFY_DEVICE;


static int check_type(HBA_PORT *port);

char* get_identify_sata(HBA_PORT *port, uint16_t *buf);

void port_reset(HBA_PORT *port);

void port_rebase(HBA_PORT *port);

void probe_port(HBA_MEM *abar);

bool read_ahci(HBA_PORT *port, uint64_t start, uint32_t count, uint8_t *buf);

bool write_ahci(HBA_PORT *port, uint64_t start, uint32_t count, uint8_t *buf);


#endif