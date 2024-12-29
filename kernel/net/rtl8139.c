#include <net/rtl8139.h>

struct nic nic = {
    .rx_buf = (uint64_t*) 0x0
};

void rtl8139_init(void) {
    linkedListNode *head = list_pci_devices;
    PCIDevice *device = NULL;

    // Find RTL8139 PCI device
    while (head) {
        if (((PCIDevice *)head->data)->deviceId == RTL8139_DEVICE_ID) {
            device = (PCIDevice *)head->data;
        }
        head = head->next;
    }
    if (!device)
        return;
    
    // bus master - set command bit 2
    uint16_t command = device->command | 0b10;
    device->command = command;
    pci_config_write_word(device->bus, device->slot, device->function, PCI_OFFSET_COMMAND, command);

    // Turn on RTL8139
    uint16_t ioaddr = (uint16_t) get_bar0(device->bus, device->slot, device->function);
    if (ioaddr & 0x1)
        ioaddr -= 1;
    nic.ioaddr = ioaddr;
    outb(ioaddr + RTL8139_CONFIG_1, 0x0);

    // Software Reset
    outb(ioaddr + RTL8139_OFFSET_CMD, RTL8139_SOFTWARE_RESET);
    while((inb(ioaddr + RTL8139_OFFSET_CMD) & RTL8139_SOFTWARE_RESET)) { }

    // Init Receive buffer
    // Note: rx_buf points to a physical address (Kernel Heap is identity mapped!)
    nic.rx_buf = (uint64_t*) malloc(8*KB + 16 + 1500);
    outl(ioaddr + RTL8139_OFFSET_RBSTART, (uint64_t) nic.rx_buf); // send uint32_t memory location to RBSTART (0x30)

    // Set IMR + ISR
    outw(ioaddr + RTL8139_OFFSET_IMR, 0x0005); // Sets the TOK and ROK bits high

    // Set Interrupt PIN and Interrupt Line
    pci_config_write_word(device->bus, device->slot, device->function, PCI_OFFSET_INTERRUPT_LINE, (RTL8139_INTERRUPT_PIN << 8) + RTL8139_INTERRUPT_LINE);
    // printf("Interrupt PIN: %d\n", get_interrupt_pin(device->bus, device->slot, device->function));
    // printf("Interrupt Line: %d\n", get_interrupt_line(device->bus, device->slot, device->function));

    // Configuring receive buffer (RCR)
    outl(ioaddr + RTL8139_OFFSET_RCR, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP

    // Enable Receive and Transmitter
    outb(ioaddr + RTL8139_OFFSET_CMD, 0x0C); // Sets the RE and TE bits high

    // Read MAC address
    rtl8139_read_mac();

    irq_clear_mask(RTL8139_INTERRUPT_LINE);

    // uint16_t status = inw(nic.ioaddr + RTL8139_OFFSET_ISR);
	// outw(nic.ioaddr + RTL8139_OFFSET_ISR, 0x05);
}

void rtl8139_read_mac(void) {
    for (uint16_t i = 0; i < sizeof(nic.mac); i++)
        nic.mac[i] = inb(nic.ioaddr + i);
}

void rtl8139_handler(uint8_t isr, uint64_t error, uint64_t irq) {
    static int i = 0;
	uint16_t status = inw(nic.ioaddr + RTL8139_OFFSET_ISR);
	outw(nic.ioaddr + RTL8139_OFFSET_ISR, 0x05);
	if(status & TOK) {
		// Sent
        printf("Sent packet!\n");
	}
	if (status & ROK) {
        ++i;
		// Received
		// receive_packet();
        printf("Received packet! %d\n", i);
	}
}
