#ifndef AHCI_H

#define AHCI_H

#include <arch/x86_64/hardwareMem.h>
#include <disk.h>
#include <pci.h>


#define AHCI_DEBUG

#ifdef AHCI_DEBUG
    #define AHCI_DEBUG_PRINT(fmt, ...) printf("%s " fmt, LOG_SYM_INF, ##__VA_ARGS__)
#else
    #define AHCI_DEBUG_PRINT(fmt, ...)
#endif

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

#define PRDT_READ_SIZE_PER_ENTRY \
    (16 * SECTOR_SIZE) // Must be dividable by the size of a sector
#define PRDT_WRITE_SIZE_PER_ENTRY \
    (16 * SECTOR_SIZE) // Must be dividable by the size of a sector


#define IDENTIFY_TIMEOUT 100000
#define READ_TIMEOUT IDENTIFY_TIMEOUT
#define WRITE_TIMEOUT READ_TIMEOUT

extern linkedListNode *ahci_devices;

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

typedef volatile struct HBA_PORT {
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

typedef struct __attribute__((packed)) {
    uint16_t general_config;            // Word 0: General configuration
    uint16_t obsolete1;                 // Word 1: Obsolete
    uint16_t specific_config;           // Word 2: Specific configuration
    uint16_t obsolete2;                 // Word 3: Obsolete
    uint16_t retired1[2];               // Words 4-5: Retired
    uint16_t obsolete3;                 // Word 6: Obsolete
    uint16_t reserved1;                 // Word 7: Reserved for Compact Flash Association (CFA)
    uint16_t retired2;                  // Word 8: Retired
    uint16_t reserved2;                 // Word 9: Reserved
    char serial_number[20];             // Words 10-19: Serial number (20 ASCII characters)
    uint16_t retired3[2];               // Words 20-21: Retired
    uint16_t obsolete4;                 // Word 22: Obsolete
    char firmware_revision[8];          // Words 23-26: Firmware revision (8 ASCII
                                        // characters)
    char model_number[40];              // Words 27-46: Model number (40 ASCII characters)
    uint16_t max_sectors_per_interrupt; // Word 47: Maximum sectors per interrupt
                                        // on R/W Multiple command
    uint16_t reserved3;                 // Word 48: Reserved
    uint16_t capabilities1;             // Word 49: Capabilities (first part)
    uint16_t capabilities2;             // Word 50: Capabilities (second part)
    uint16_t obsolete5[2];              // Words 51-52: Obsolete
    uint16_t validity_flags;            // Word 53: Validity of fields (54-58)
    uint16_t
        curr_logical_cylinders;              // Word 54: Number of current logical cylinders
    uint16_t curr_logical_heads;             // Word 55: Number of current logical heads
    uint16_t curr_logical_sectors_per_track; // Word 56: Number of current logical
                                             // sectors per track
    uint32_t curr_capacity_in_sectors;       // Words 57-58: Current capacity in sectors
    uint16_t multi_sector_setting;           // Word 59: Multi-sector setting
    uint32_t total_user_addressable_sectors; // Words 60-61: Total user
                                             // addressable sectors
    uint16_t single_word_dma;                // Word 62: Single word DMA modes
    uint16_t multi_word_dma;                 // Word 63: Multi-word DMA modes
    uint16_t pio_modes;                      // Word 64: PIO modes supported
    uint16_t min_multiword_dma_cycle_time;   // Word 65: Minimum Multiword DMA
                                             // transfer cycle time
    uint16_t rec_multiword_dma_cycle_time;   // Word 66: Recommended Multiword DMA
                                             // transfer cycle time
    uint16_t min_pio_cycle_time_no_flow;     // Word 67: Minimum PIO transfer cycle
                                             // time without flow control
    uint16_t min_pio_cycle_time_with_flow;   // Word 68: Minimum PIO transfer cycle
                                             // time with IORDY flow control
    uint16_t reserved4[6];                   // Words 69-74: Reserved
    uint16_t queue_depth;                    // Word 75: Queue depth
    uint16_t sata_capabilities;              // Word 76: SATA capabilities
    uint16_t
        sata_additional_capabilities; // Word 77: SATA additional capabilities
    uint16_t sata_features_supported; // Word 78: SATA features supported
    uint16_t sata_features_enabled;   // Word 79: SATA features enabled
    uint16_t major_version;           // Word 80: Major version number
    uint16_t minor_version;           // Word 81: Minor version number
    uint16_t cmd_set_support;         // Word 82: Command set supported (first part)
    uint16_t cmd_set_support2;        // Word 83: Command set supported (second part)
    uint16_t
        cmd_set_feature_extn;        // Word 84: Command set/feature supported extension
    uint16_t cmd_set_feature_enable; // Word 85: Command set/feature enabled
    uint16_t
        cmd_set_feature_enable2;      // Word 86: Command set/feature enabled extension
    uint16_t cmd_set_feature_default; // Word 87: Command set/feature default
    uint16_t ultra_dma_modes;         // Word 88: Ultra DMA modes supported and selected
    uint16_t reserved5[38];           // Words 89-126: Reserved
    uint16_t hardware_reset_result;   // Word 127: Hardware reset result
    uint16_t reserved6[128];          // Words 128-255: Reserved
} ATA_IDENTIFY_DEVICE;

typedef struct __attribute__((packed)) {
    unsigned short config;       /* lots of obsolete bit flags */
    unsigned short cyls;         /* obsolete */
    unsigned short reserved2;    /* special config */
    unsigned short heads;        /* "physical" heads */
    unsigned short track_bytes;  /* unformatted bytes per track */
    unsigned short sector_bytes; /* unformatted bytes per sector */
    unsigned short sectors;      /* "physical" sectors per track */
    unsigned short vendor0;      /* vendor unique */
    unsigned short vendor1;      /* vendor unique */
    unsigned short vendor2;      /* vendor unique */
    unsigned char serial_no[20]; /* 0 = not_specified */
    unsigned short buf_type;
    unsigned short buf_size;    /* 512 byte increments; 0 = not_specified */
    unsigned short ecc_bytes;   /* for r/w long cmds; 0 = not_specified */
    unsigned char fw_rev[8];    /* 0 = not_specified */
    unsigned char model[40];    /* 0 = not_specified */
    unsigned short multi_count; /* Multiple Count */
    unsigned short dword_io;    /* 0=not_implemented; 1=implemented */
    unsigned short capability1; /* vendor unique */
    unsigned short
        capability2;                  /* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup word: 50 */
    unsigned char vendor5;            /* vendor unique */
    unsigned char tPIO;               /* 0=slow, 1=medium, 2=fast */
    unsigned char vendor6;            /* vendor unique */
    unsigned char tDMA;               /* 0=slow, 1=medium, 2=fast */
    unsigned short field_valid;       /* bits 0:cur_ok 1:eide_ok */
    unsigned short cur_cyls;          /* logical cylinders */
    unsigned short cur_heads;         /* logical heads word 55*/
    unsigned short cur_sectors;       /* logical sectors per track */
    unsigned short cur_capacity0;     /* logical total sectors on drive */
    unsigned short cur_capacity1;     /*  (2 words, misaligned int)     */
    unsigned char multsect;           /* current multiple sector count */
    unsigned char multsect_valid;     /* when (bit0==1) multsect is ok */
    unsigned int lba_capacity;        /* total number of sectors */
    unsigned short dma_1word;         /* single-word dma info */
    unsigned short dma_mword;         /* multiple-word dma info */
    unsigned short eide_pio_modes;    /* bits 0:mode3 1:mode4 */
    unsigned short eide_dma_min;      /* min mword dma cycle time (ns) */
    unsigned short eide_dma_time;     /* recommended mword dma cycle time (ns) */
    unsigned short eide_pio;          /* min cycle time (ns), no IORDY  */
    unsigned short eide_pio_iordy;    /* min cycle time (ns), with IORDY */
    unsigned short words69_70[2];     /* reserved words 69-70 */
    unsigned short words71_74[4];     /* reserved words 71-74 */
    unsigned short queue_depth;       /*  */
    unsigned short sata_capability;   /*  SATA Capabilities word 76*/
    unsigned short sata_additional;   /*  Additional Capabilities */
    unsigned short sata_supported;    /* SATA Features supported  */
    unsigned short features_enabled;  /* SATA features enabled */
    unsigned short major_rev_num;     /*  Major rev number word 80 */
    unsigned short minor_rev_num;     /*  */
    unsigned short command_set_1;     /* bits 0:Smart 1:Security 2:Removable 3:PM */
    unsigned short command_set_2;     /* bits 14:Smart Enabled 13:0 zero */
    unsigned short cfsse;             /* command set-feature supported extensions */
    unsigned short cfs_enable_1;      /* command set-feature enabled */
    unsigned short cfs_enable_2;      /* command set-feature enabled */
    unsigned short csf_default;       /* command set-feature default */
    unsigned short dma_ultra;         /*  */
    unsigned short word89;            /* reserved (word 89) */
    unsigned short word90;            /* reserved (word 90) */
    unsigned short CurAPMvalues;      /* current APM values */
    unsigned short word92;            /* reserved (word 92) */
    unsigned short comreset;          /* should be cleared to 0 */
    unsigned short accoustic;         /*  accoustic management */
    unsigned short min_req_sz;        /* Stream minimum required size */
    unsigned short transfer_time_dma; /* Streaming Transfer Time-DMA */
    unsigned short access_latency;    /* Streaming access latency-DMA & PIO WORD 97*/
    unsigned int perf_granularity;    /* Streaming performance granularity */
    unsigned int
        total_usr_sectors[2];          /* Total number of user addressable sectors */
    unsigned short transfer_time_pio;  /* Streaming Transfer time PIO */
    unsigned short reserved105;        /* Word 105 */
    unsigned short sector_sz;          /* Puysical Sector size / Logical sector size */
    unsigned short inter_seek_delay;   /* In microseconds */
    unsigned short words108_116[9];    /*  */
    unsigned int words_per_sector;     /* words per logical sectors */
    unsigned short supported_settings; /* continued from words 82-84 */
    unsigned short command_set_3;      /* continued from words 85-87 */
    unsigned short words121_126[6];    /* reserved words 121-126 */
    unsigned short word127;            /* reserved (word 127) */
    unsigned short security_status;    /* device lock function
                                        * 15:9	reserved
                                        * 8	security level 1:max 0:high
                                        * 7:6	reserved
                                        * 5	enhanced erase
                                        * 4	expire
                                        * 3	frozen
                                        * 2	locked
                                        * 1	en/disabled
                                        * 0	capability
                                        */
    unsigned short csfo;               /* current set features options
                                        * 15:4	reserved
                                        * 3	auto reassign
                                        * 2	reverting
                                        * 1	read-look-ahead
                                        * 0	write cache
                                        */
    unsigned short words130_155[26];   /* reserved vendor words 130-155 */
    unsigned short word156;
    unsigned short words157_159[3];  /* reserved vendor words 157-159 */
    unsigned short cfa;              /* CFA Power mode 1 */
    unsigned short words161_175[15]; /* Reserved */
    unsigned char
        media_serial[60];             /* words 176-205 Current Media serial number */
    unsigned short sct_cmd_transport; /* SCT Command Transport */
    unsigned short words207_208[2];   /* reserved */
    unsigned short
        block_align;                    /* Alignement of logical blocks in larger physical blocks */
    unsigned int WRV_sec_count;         /* Write-Read-Verify sector count mode 3 only */
    unsigned int verf_sec_count;        /* Verify Sector count mode 2 only */
    unsigned short nv_cache_capability; /* NV Cache capabilities */
    unsigned short nv_cache_sz;         /* NV Cache size in logical blocks */
    unsigned short nv_cache_sz2;        /* NV Cache size in logical blocks */
    unsigned short rotation_rate;       /* Nominal media rotation rate */
    unsigned short reserved218;         /*  */
    unsigned short nv_cache_options;    /* NV Cache options */
    unsigned short words220_221[2];     /* reserved */
    unsigned short transport_major_rev; /*  */
    unsigned short transport_minor_rev; /*  */
    unsigned short words224_233[10];    /* Reserved */
    unsigned short
        min_dwnload_blocks; /* Minimum number of 512byte units per DOWNLOAD
                               MICROCODE command for mode 03h */
    unsigned short
        max_dwnload_blocks;          /* Maximum number of 512byte units per DOWNLOAD
                                        MICROCODE command for mode 03h */
    unsigned short words236_254[19]; /* Reserved */
    unsigned short integrity;        /* Cheksum, Signature */
} UNNAMED_STRUCT;

/** @brief This function sets up all ahci disks and adds them to the linked list of
drives*/
void setup_ahci_controllers();

/** 
* @brief Find the disks type
* @arg[in] port
* @return The disks type
*/
int check_type(HBA_PORT *port);

/** 
* @brief Get the disks ata identify that
* @arg[in] port
* @arg[out] buffer The buffer which the disks data will be put into 
* @return If the operation was a success
*/
bool get_identify_sata(volatile HBA_PORT *port, uint16_t *buf);

// Reset the port via the ports registers
void port_reset(HBA_PORT *port);

// Allocate memory for the port and 
void port_rebase(HBA_PORT *port);

// Iterate on all implemented ports
void probe_port(HBA_MEM *abar);

/**
* @brief Read a count number of sectors from a port(Disk) from the buffer
* @param[in] port The port/disk
* @param[in] start The offset in sectors
* @param[in] count Number of buffers to read
* @param[out] buf Buffer which will contain the data from the disk (Should be at a size of at least count * size of a sector) 

* @return If the operation was successful

*/
bool read_ahci(volatile HBA_PORT *port, uint64_t start, uint32_t count, uint8_t *buf);

/**
* @brief Write a count number of sectors from a port(Disk) from the buffer
* @param[in] port The port/disk
* @param[in] start The offset in sectors
* @param[in] count The number of sectors to write
* @param[in] buf The 

* @return If the operation was successful

*/
bool write_ahci(volatile HBA_PORT *port, uint64_t start, uint32_t count, uint8_t *buf);

#endif