#include <ahci.h>

linkedListNode* setup_ahci_controllers() {
  linkedListNode *head = listPCIDevices;
  linkedListNode* out = NULL;
  while (head) {
    PCIDevice *device = (PCIDevice *)head->data;
    if ((device->classCode == 0x06 || device->classCode == 0x01) && device->subclass == 0x06) {
      assign_bar(*device, AHCI_CONFIG_BAR_NUM);
      printf("Found AHCI controller %x:%x", device->bus, device->slot);
      append_node(&out, device);
    }
    head = head->next;
  }
  return out;
}