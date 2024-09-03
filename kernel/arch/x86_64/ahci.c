#include <ahci.h>

linkedListNode* ahci_devices = NULL;

void setup_ahci_controllers() {
  linkedListNode *head = listPCIDevices;
  while (head) {
    PCIDevice *device = (PCIDevice *)head->data;
    if ((device->classCode == 0x06 || device->classCode == 0x01) && device->subclass == 0x06) {
      assign_bar(*device, AHCI_CONFIG_BAR_NUM);
      append_node(&ahci_devices, device);
    }
    head = head->next;
  }
}