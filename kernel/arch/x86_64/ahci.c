#include <ahci.h>
#include <stdio.h>
#include <pci.h>

linkedListNode* ahci_devices = NULL;

void initialize_ahci(HBA_MEM *abar) {
    // Enable AHCI by setting the AHCI Enable (AE) bit in the Global Host Control (GHC) register
    abar->ghc |= (1 << 31); // Set the AHCI Enable bit

    // Wait for AHCI controller to become ready
    while (!(abar->ghc & (1 << 31))) {
		printf("Ahci not ready");
    }
    
    // Check capabilities and configure ports
    uint32_t cap = abar->cap;
	printf("AHCI GHC: 0x%x\n", abar->ghc);
    printf("AHCI Capabilities: 0x%x\n", cap);

    // Example: Check and initialize ports if needed
}

void setup_ahci_controllers() {
  linkedListNode *head = listPCIDevices;
  while (head) {
    PCIDevice *device = (PCIDevice *)head->data;
    if ((device->classCode == 0x06 || device->classCode == 0x01) && device->subclass == 0x06) {
      HBA_MEM* abar = assign_bar(*device, AHCI_CONFIG_BAR_NUM);
	  printf("Abar addr: %x\n", abar);
      initialize_ahci(abar);
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