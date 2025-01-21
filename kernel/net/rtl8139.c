#include <net/rtl8139.h>
// See: https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139D_DataSheet.pdf
// See: https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139_ProgrammersGuide.pdf

struct nic g_nic = {
    .rx_buf = (uint32_t *)0x0,
    .ioaddr = 0x0,
    .mac = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    .trrc = 0x0,
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
    g_nic.ioaddr = ioaddr;
    outb(ioaddr + RTL8139_OFFSET_CONFIG_1, 0x0);

    // Software Reset
    outb(ioaddr + RTL8139_OFFSET_CMD, RTL8139_SOFTWARE_RESET);
    while ((inb(ioaddr + RTL8139_OFFSET_CMD) & RTL8139_SOFTWARE_RESET)) {}

    // Init Receive buffer
    // Note: rx_buf points to a physical address (Kernel Heap is identity mapped!)
    g_nic.rx_buf = (uint32_t *) malloc(RX_BUFFER_LEN);
    outl(ioaddr + RTL8139_OFFSET_RBSTART, (uint32_t)((uint64_t)g_nic.rx_buf)); // send uint32_t memory location to RBSTART (0x30)

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
    print_mac(g_nic.mac);
    printf("\n");

    // Enable RTL8139 NIC IRQ
    irq_clear_mask(RTL8139_INTERRUPT_LINE);
    printf("FIN INIT\n");
}

void rtl8139_read_self_mac(void) {
    for (uint16_t i = 0; i < sizeof(g_nic.mac); i++)
        g_nic.mac[i] = inb(g_nic.ioaddr + RTL8139_OFFSET_MAC0 + i);
}

void handle_packet(struct packet *pkt) {
    if (pkt->len <= 0)
        return;

    irq_set_mask(RTL8139_INTERRUPT_LINE);

    // Parse Ethernet header
    struct ethernet_header *eth_header = (struct ethernet_header *)pkt->data;
    eth_header->type = be16toh(eth_header->type);
    printf("GOT PACKET\n");

    // Check if it's an ARP packet
    if (eth_header->type == PTYPE_ARP) {
        struct arp_header *arp = (struct arp_header *)(pkt->data + sizeof(struct ethernet_header));

        printf("GOT ARP PACKET\n");
        // Check if it's an ARP reply
        if (htobe16(arp->oper) == ARP_OPER_REPLY) {
            print_ipv4(arp->spa);
            printf(" is at ");
            print_mac(arp->sha);
            printf("\n");

            // Cache the MAC address for the given IP
            // cache_arp_entry(arp->spa, arp->sha);
        }
        else if (htobe16(arp->oper) == ARP_OPER_REQUEST) {
            if (!memcmp((void *)arp->tha, (void *)g_nic.mac, 6) ||
                !memcmp((void *)arp->sha, (void *)g_nic.mac, 6)) {
                printf("Who has ");
                print_ipv4(arp->tpa);
                printf("? Tell ");
                print_ipv4(arp->spa);
                printf("\n");
            }
        }
    }

    irq_clear_mask(RTL8139_INTERRUPT_LINE);
}

void receive_packet() {
    static uint32_t rx_offset = 0; // Tracks where to read next (CAPR)

    // Read packet metadata (header, length, data)
    struct packet pkt = {
        .header = (uint8_t *)(uint64_t)(((uint32_t)(uint64_t)g_nic.rx_buf) + rx_offset),
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
    outw(g_nic.ioaddr + RTL8139_OFFSET_CAPR, (rx_offset - 16) & 0xFFFF); // RTL8139 expects offset - 16
}

void send_packet(void *packet, int packet_len) {
    // Transmit Start = packet pointer
    outl(g_nic.ioaddr + RTL8139_OFFSET_TRANSMIT_BUF_0 + 4*g_nic.trrc, (uint32_t)(uint64_t)packet);

    // Transmit Length = packet length (and zero OWN bit)
    outl(g_nic.ioaddr + RTL8139_OFFSET_TRANSMIT_CMD_0 + 4*g_nic.trrc, packet_len);

    // Wait for Transmit OK
    while (!(inl(g_nic.ioaddr + RTL8139_OFFSET_TRANSMIT_CMD_0 + 4*g_nic.trrc) & (0x1 << RTL8139_OFFSET_TRANSMIT_OK))) {}

    g_nic.trrc++;
    g_nic.trrc %= 4;
}

void rtl8139_handler(__attribute__((unused)) uint8_t isr, __attribute__((unused)) uint64_t error, __attribute__((unused)) uint64_t irq) {
    // Packet status
    uint16_t status = inw(g_nic.ioaddr + RTL8139_OFFSET_ISR);

    // Acknowledge the interrupt
    outw(g_nic.ioaddr + RTL8139_OFFSET_ISR, RTL8139_ALLOW_ROK_TOK);

    static int receive_cnt = 0;

    if (status & ROK) { // Received
        receive_cnt++;
        receive_packet();
    }
    if (status & ROK && receive_cnt == 5) {
        char spa[IPV4_ADDR_SIZE] = {10, 0, 0, 138};
        // char hwdst[MAC_ADDR_SIZE] = {0xe0, 0x4e, 0x7a, 0x13, 0x75, 0x19};
        char hwdst[MAC_ADDR_SIZE] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        char tpa[IPV4_ADDR_SIZE] = {10, 0, 0, 31};
        send_arp(ARP_OPER_REQUEST, g_nic.mac, spa, hwdst, tpa);
    }
    if(status & TOK) { // Sent
        // printf("Packet sent!\n");
    }
}


