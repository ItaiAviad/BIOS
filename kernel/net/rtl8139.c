#include <net/rtl8139.h>
// See: https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139D_DataSheet.pdf

struct nic nic = {
    .rx_buf = (uint32_t *)0x0,
    .ioaddr = 0x0,
    .mac.octets = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
};

void rtl8139_init(void) {
    linkedListNode *head = list_pci_devices;
    PCIDevice *device = NULL;

    // Find RTL8139 PCI device
    while (head) {
        if (((PCIDevice *)head->data)->deviceId == RTL8139_DEVICE_ID)
            device = (PCIDevice *)head->data;
        head = head->next;
    }
    if (!device)
        return;

    // bus master - set command bit 2, see: https://wiki.osdev.org/PCI#Command_Register
    device->command |= 0b100;
    pci_config_write_word(device->bus, device->slot, device->function, PCI_OFFSET_COMMAND, device->command);

    // Turn on RTL8139
    uint16_t ioaddr = (uint16_t)get_bar0(device->bus, device->slot, device->function);
    if (ioaddr & 0x1)
        ioaddr -= 1;
    nic.ioaddr = ioaddr;
    outb(ioaddr + RTL8139_OFFSET_CONFIG_1, 0x0);

    // Software Reset
    outb(ioaddr + RTL8139_OFFSET_CMD, RTL8139_SOFTWARE_RESET);
    while ((inb(ioaddr + RTL8139_OFFSET_CMD) & RTL8139_SOFTWARE_RESET)) {
    }

    // Init Receive buffer
    // Note: rx_buf points to a physical address (Kernel Heap is identity mapped!)
    nic.rx_buf = (uint32_t *) malloc(RX_BUFFER_LEN);
    outl(ioaddr + RTL8139_OFFSET_RBSTART, (uint32_t)((uint64_t)nic.rx_buf)); // send uint32_t memory location to RBSTART (0x30)

    // Set IMR + ISR
    outw(ioaddr + RTL8139_OFFSET_IMR, RTL8139_ALLOW_ROK_TOK); // Sets the TOK and ROK bits high

    // Set Interrupt PIN and Interrupt Line
    pci_config_write_word(device->bus, device->slot, device->function, PCI_OFFSET_INTERRUPT_LINE, (RTL8139_INTERRUPT_PIN << 8) + RTL8139_INTERRUPT_LINE);

    // Configuring receive buffer (RCR)
    outl(ioaddr + RTL8139_OFFSET_RCR, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP

    // Enable Receive and Transmitter
    outb(ioaddr + RTL8139_OFFSET_CMD, 0b1100); // Sets the RE and TE bits high

    // Read MAC address
    rtl8139_read_self_mac();
    print_mac(nic.mac);

    // Enable RTL8139 NIC IRQ
    irq_clear_mask(RTL8139_INTERRUPT_LINE);
}

void rtl8139_read_self_mac(void) {
    for (uint16_t i = 0; i < sizeof(nic.mac); i++)
        nic.mac.octets[i] = inb(nic.ioaddr + RTL8139_OFFSET_MAC0 + i);
}

void handle_packet(struct eth_packet *pkt) {
    // printf("Received packet of length %d\n", pkt->len);
    // Dump the first few bytes
    // for (int i = 0; i < pkt->len && i < 20; i++) {
    //     printf("%x ", pkt->data[i]);
    // }
    // printf("\n");
    if (pkt->len <= 0)
        return;
    
    struct ethernet *eth = malloc(sizeof(struct ethernet));
    memcpy(eth, pkt->data, sizeof(struct ethernet));
    mac_reverse_type_endian(eth);

    print_mac(eth->mac_dst);
    printf(" ");
    print_mac(eth->mac_src);
    printf(", type/len: %x\n", eth->type);
}

void receive_packet() {
    static uint32_t rx_offset = 0; // Tracks where to read next (CAPR)

    // Read packet metadata (header, length, data)
    struct eth_packet pkt = {
        .header = (uint8_t *)(uint64_t)(((uint32_t)(uint64_t)nic.rx_buf) + rx_offset),
        .len = *((uint16_t *)(pkt.header + 2)) - RTL8139_RX_CRC_LEN, // CRC is included in len, subtract it
        .data = (uint8_t *)(pkt.header + 4),
    };
    if (pkt.len <= 4)
        return;

    handle_packet(&pkt);

    // Advance to the next packet (wrap if necessary)
    rx_offset = (rx_offset + pkt.len + (8 + 3)) & ~3;                 // Align to 4-byte boundary
    rx_offset -= rx_offset >= RX_BUFFER_SIZE ? RX_BUFFER_SIZE : 0; // Calculate offset in buffer
    
    // Acknowledge the packet by updating CAPR (Current Address of Packet Read)
    outw(nic.ioaddr + RTL8139_OFFSET_CAPR, (rx_offset - 16) & 0xFFFF); // RTL8139 expects offset - 16
}

void rtl8139_handler(__attribute__((unused)) uint8_t isr, __attribute__((unused)) uint64_t error, __attribute__((unused)) uint64_t irq) {
    uint16_t status = inw(nic.ioaddr + RTL8139_OFFSET_ISR);
    outw(nic.ioaddr + RTL8139_OFFSET_ISR, RTL8139_ALLOW_ROK_TOK);

    printf("status: %b\n", status);
    if (status & ROK) { // Received
        receive_packet();
    }
    // if(status & TOK) { // Sent
    //     printf("Sent packet!\n");
    // }
}
