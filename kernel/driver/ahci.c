#include "arch/x86_64/hardwareMem.h"
#include "arch/x86_64/io.h"
#include <arch/x86_64/hardwareMem.h>
#include "math.h"
#include <ahci.h>
#include <disk.h>
#include <pci.h>

#include <stdio.h>
#include <string.h>

#define AHCI_PCI_CMD_REG_BITS 0b11

linkedListNode *ahci_devices = NULL;
size_t cmdslots;

void initialize_ahci(HBA_MEM *abar, __attribute__((unused)) PCIDevice *device) {

    while ((abar->bohc & 0b1) == 1) {
        printf("Ahci not ready, value: %d\n", abar->bohc);
    }

    #ifdef DEBUG
    printf("Bios to os hand over done value:%d\n", abar->bohc);
    #endif

    abar->bohc |= 0b10;

    while(abar->bohc & 0b10 ){
        printf("Abar still reseting\n");
    }

    // Enable AHCI by setting the AHCI Enable (AE) bit in the Global Host Control
    // (GHC) register Enable Interrupts by setting the Interrupt Enable (IE) bit
    // in the Global Host Control (GHC) register
    abar->ghc |= (1 << 31) | 0b10;
    while (!(abar->ghc & (1 << 31))) {
        printf("Ahci not ready, value: %d\n", abar->ghc & (1 << 31));
    }

    // Check capabilities and configure ports
    printf("%s AHCI\n", LOG_SYM_SUC);
    #ifdef DEBUG
    printf("AHCI GHC: %d\n", abar->ghc);
    printf("AHCI Capabilities: %d\n", abar->cap);
    #endif

    cmdslots = ((abar->cap >> 8) & 0x1F) + 1;
}

// This function sets up all ahci disks and adds them to the linked list of
// drives
void setup_ahci_controllers() {
    linkedListNode *head = list_pci_devices;
    while (head) {
        PCIDevice *device = (PCIDevice *)head->data;
        if ((device->classCode == 0x06 || device->classCode == 0x01) &&
            (device->subclass == 0x06 || device->subclass == 0x01)) {
            HBA_MEM *abar = assign_bar(*device, AHCI_CONFIG_BAR_NUM);
            if (abar == NULL) {
                head = head->next;
                continue;
            }
            #ifdef DEBUG
            printf("Abar addr: %d\n", abar);
            #endif
            uint16_t command_reg = pci_config_read_dword(
                device->bus, device->slot, device->function, PCI_OFFSET_COMMAND);
            command_reg |= (uint16_t)AHCI_PCI_CMD_REG_BITS;
            pci_config_write_dword(device->bus, device->slot, device->function,
                                   PCI_OFFSET_COMMAND, command_reg);
            pci_config_write_dword(device->bus, device->slot, device->function,
                                   PCI_OFFSET_INTERRUPT_LINE, 2);
            initialize_ahci(abar, device);
            probe_port(abar);
            hardware_append_node(&ahci_devices, device);
        }
        head = head->next;
    }
}

// Check device type returns the ata device type at the given port
int check_type(HBA_PORT *port) {
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;

    if (det != HBA_PORT_DET_PRESENT) // Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;

    switch (port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}

void probe_port(HBA_MEM *abar) {
    // Search disk in implemented ports
    uint32_t pi = abar->pi;
    #ifdef DEBUG
    printf("Pi: %d\n", pi);
    #endif
    int i = 0;
    while (i < 32) {
        if (pi & 1) {
            port_reset((HBA_PORT *)abar->ports + i);
            int dt = check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA) {
                printf("%s SATA drive found at port %d\n", LOG_SYM_INF, i);
                port_rebase(abar->ports + i);

                ATA_IDENTIFY_DEVICE* ata_id = hardware_allocate_mem(sizeof(ATA_IDENTIFY_DEVICE), 0);
                get_identify_sata(abar->ports + i, (uint16_t*) ata_id);

                disk *disk_curr = hardware_allocate_mem(sizeof(disk), 0);

                disk_curr->disk_id = (last_disk_id++);
                disk_curr->disk_size = ata_id->total_user_addressable_sectors * SECTOR_SIZE;
                disk_curr->disk_type = AHCI_SATA;
                disk_curr->drive_data.ahci_drive_data.port = (struct HBA_PORT*) (abar->ports + i);
                disk_curr->drive_data.ahci_drive_data.ahci_bar = (struct HBA_MEM*) abar;

                hardware_append_node(&list_drives, disk_curr);

            } else if (dt == AHCI_DEV_SATAPI) {
                printf("SATAPI drive found at port %d\n", i);
            } else if (dt == AHCI_DEV_SEMB) {
                printf("SEMB drive found at port %d\n", i);
            } else if (dt == AHCI_DEV_PM) {
                printf("PM drive found at port %d\n", i);
            } else {
                #ifdef DEBUG
                printf("No drive found at port %d\n", i);
                #endif
            }
        }

        pi >>= 1;
        i++;
    }
}

// Start command engine
void start_cmd(HBA_PORT *port) {
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR)
        ;

    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}


// Send reset to a port
void port_reset(HBA_PORT *port) {
    port->sctl = 0x301;
    port->sctl = 0x300;
}

// Stop command engine
void stop_cmd(HBA_PORT *port) {
    port_reset(port);
    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;

    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;

    // Wait until FR (bit14), CR (bit15) are cleared
    while (1) {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }
}


// Allocate memory for the port(Disk)
void port_rebase(HBA_PORT *port) {
    stop_cmd(port); // Stop command engine

    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port

    port->clb = (uint64_t)hardware_allocate_mem(0x400, 0x400);
    memset((void *)(port->clb), 0, 0x400);

    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = (uint64_t)hardware_allocate_mem(0x100, 0x100);
    memset((void *)(port->fb), 0, 0x100);

    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)(port->clb);
    for (int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
                                // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = (uint64_t)hardware_allocate_mem(0x100, 0x100);
        memset((void *)cmdheader[i].ctba, 0, 0x100);
    }

    start_cmd(port); // Start command engine
}

// Find a free command list slot
int find_cmdslot(HBA_PORT *port) {
    // If not set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    for (size_t i = 0; i < cmdslots; i++) {
        if ((slots & 1) == 0)
            return i;
        slots >>= 1;
    }
    printf("Cannot find free command list entry\n");
    return -1;
}

bool get_identify_sata(volatile HBA_PORT *port, uint16_t *buf) {
    port->is = (uint32_t)-1; // Clear pending interrupt bits
    int spin = 0;            // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return false;

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // Command FIS size
    cmdheader->w = 0;                                        // Read from device
    cmdheader->prdtl = 1;

    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);
    memset(cmdtbl, 0,
           sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

    cmdtbl->prdt_entry[0].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[0].dbc = sizeof(ATA_IDENTIFY_DEVICE) - 1;

    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);
    memset(cmdfis, 0, sizeof(cmdfis));

    cmdfis->fis_type = FIS_TYPE_REG_H2D; // FIs type - host to device
    cmdfis->c = 1; // Command
    cmdfis->device = 1 << 6;
    cmdfis->command = ATA_CMD_IDENTIFY;

    // The below loop waits until the port is no longer busy before issuing a new
    // command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ))) {
        spin++;
        if (spin == 1000000) {
            printf("Port is hung\n");
        }
        io_wait();
    }

    port->ci = 1 << slot; // Issue command

    // Wait for completion
    while (1) {
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is & HBA_PxIS_TFES) // Task file error
        {
            printf("Identify disk error\n");
            return 0;
        }
    }

    // Check again
    if (port->is & HBA_PxIS_TFES) {
        printf("Identify disk error\n");
        return 0;
    }

    return true;
}

bool write_ahci(volatile HBA_PORT *port, uint64_t start, uint32_t count, uint8_t *buf) {
    port->is = (uint32_t)-1; // Clear pending interrupt bits
    int spin = 0;            // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return false;

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // Command FIS size
    cmdheader->w = 1;                                        // Write to device
    cmdheader->prdtl = upper_divide(count, PRDT_WRITE_SIZE_PER_ENTRY /
                                               SECTOR_SIZE); // PRDT entries count

    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);
    memset(cmdtbl, 0,
           sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

    // 8K bytes (16 sectors) per PRDT
    int i, count_temp = count;
    for (i = 0; i < cmdheader->prdtl - 1; i++) {
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc =
            PRDT_WRITE_SIZE_PER_ENTRY - 1; // Number of bytes per prdt
        cmdtbl->prdt_entry[i].i = 1;
        buf += PRDT_WRITE_SIZE_PER_ENTRY;
        count_temp -= PRDT_WRITE_SIZE_PER_ENTRY / SECTOR_SIZE;
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc =
        (count_temp << 9) - 1; // Calculate number of sectors remaining.

    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;

    cmdfis->lba0 = (uint8_t)start;
    cmdfis->lba1 = (uint8_t)(start >> 8);
    cmdfis->lba2 = (uint8_t)(start >> 16);
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba3 = (uint8_t)(start >> 24);
    cmdfis->lba4 = (uint8_t)(start >> 32);
    cmdfis->lba5 = (uint8_t)(start >> 40);

    cmdfis->count = count;

    // The below loop waits until the port is no longer busy before issuing a new
    // command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ))) {
        spin++;
        if (spin == 1000000) {
            printf("Port is hung\n");
        }
        io_wait();
    }

    port->ci = 1 << slot; // Issue command

    // Wait for completion
    while (1) {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is & HBA_PxIS_TFES) // Task file error
        {
            printf("Write disk error\n");
            return 0;
        }
    }

    // Check again
    if (port->is & HBA_PxIS_TFES) {
        printf("Write disk error\n");
        return 0;
    }

    return true;
}

bool read_ahci(volatile HBA_PORT *port, uint64_t start, uint32_t count, uint8_t *buf)
{
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return false;
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = upper_divide(count, PRDT_READ_SIZE_PER_ENTRY /
                                               SECTOR_SIZE);	// PRDT entries count
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
	// 8K bytes (16 sectors) per PRDT
	int i, count_temp = count;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = PRDT_READ_SIZE_PER_ENTRY-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += PRDT_READ_SIZE_PER_ENTRY;
		count_temp -= PRDT_READ_SIZE_PER_ENTRY / SECTOR_SIZE;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count_temp<<9)-1;
	cmdtbl->prdt_entry[i].i = 1;
	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
	cmdfis->lba0 = (uint8_t)start;
	cmdfis->lba1 = (uint8_t)(start>>8);
	cmdfis->lba2 = (uint8_t)(start>>16);
	cmdfis->device = 1<<6;	// LBA mode
	cmdfis->lba3 = (uint8_t)(start>>24);
	cmdfis->lba4 = (uint8_t)(start >> 32);
	cmdfis->lba5 = (uint8_t)(start >> 40);
	cmdfis->count = count;
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)))
	{
		spin++;
		if (spin == 1000000)
		{
			printf("Port is hung\n");
			// stop_cmd(port);
			// start_cmd(port);
		}
		io_wait();
	}
	port->ci = 1<<slot;	// Issue command
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			printf("Read disk error\n");
			return 0;
		}
	}
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		printf("Read disk error\n");
		return 0;
	}
	return true;
}