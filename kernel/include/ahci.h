#include <dataStructrures/linkedList.h>
#include <pci.h>
#include <stdio.h>

#ifndef AHCI_H

#define AHCI_H

#define AHCI_CONFIG_BAR_NUM 5

linkedListNode* setup_ahci_controllers();

#endif