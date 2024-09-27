#include "arch/x86_64/io.h"
#include <ahci.h>
#include <stdint.h>
#include <stdio.h>
#include <pci.h>

linkedListNode* ahci_devices = NULL;
#define AHCI_PCI_CMD_REG_BITS 0b11

void volatile initialize_ahci(HBA_MEM *abar, PCIDevice* device) {
    // Enable AHCI by setting the AHCI Enable (AE) bit in the Global Host Control (GHC) register

	/*abar->bohc = 0b10;


	while (!((abar->bohc & 0xFF) == 0b10)) {
		printf("Ahci not ready, value: %d", abar->bohc);
    }

	printf("Bios to os hand over done.\n");*/


	/*abar->ghc |= 0b1;
    // Wait for AHCI controller to become ready
    while (abar->ghc & 0b1) {
		printf("Ahci not ready, value: %d", abar->ghc);
    }

	printf("Ahci reset complete.\n");*/


	abar->ghc |= (1 << 31);
	while (!(abar->ghc & (1 << 31))) {
		printf("Ahci not ready, value: %d", abar->ghc & (1 << 31));
    }
	
    
    // Check capabilities and configure ports
	printf("AHCI GHC: 0x%x\n", abar->ghc);
    printf("AHCI Capabilities: 0x%x\n", abar->cap);

    // Example: Check and initialize ports if needed
}

// This function is responsible for setting up all pci devices and registering them in a linked list.
void setup_ahci_controllers() {
  linkedListNode *head = listPCIDevices;
  while (head) {
    PCIDevice *device = (PCIDevice *)head->data;
    if ((device->classCode == 0x06 || device->classCode == 0x01) && (device->subclass == 0x06 || device->subclass == 0x01)) {
      HBA_MEM* abar = assign_bar(*device, AHCI_CONFIG_BAR_NUM);
	  if(abar == NULL){
		continue;
	  }
	  printf("Abar addr: %d\n", abar);
	  uint16_t command_reg = pci_config_read_dword(device->bus, device->slot, device->function, PCI_OFFSET_COMMAND);
	  command_reg |= (uint16_t)AHCI_PCI_CMD_REG_BITS;
	  pci_config_write_dword(device->bus, device->slot, device->function, PCI_OFFSET_COMMAND, command_reg);
	  pci_config_write_dword(device->bus, device->slot, device->function, PCI_OFFSET_INTERRUPT_LINE, 2);
      initialize_ahci(abar, device);
      probe_port(abar);
      append_node(&ahci_devices, device);
    }
    head = head->next;
  }
}

// Check device type
static int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch (port->sig)
	{
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

void probe_port(HBA_MEM *abar)
{
	// Search disk in implemented ports
	uint32_t pi = abar->pi;
	printf("Pi: %x", pi);
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				printf("SATA drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				printf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				printf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				printf("PM drive found at port %d\n", i);
			}
			else
			{
				printf("No drive found at port %d\n", i);
			}
		}

		pi >>= 1;
		i ++;
	}
}