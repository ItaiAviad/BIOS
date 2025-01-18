#include <net/arp.h>
#include <net/rtl8139.h>

void send_arp(uint16_t oper, char sha[MAC_ADDR_SIZE], char spa[IPV4_ADDR_SIZE], char tha[MAC_ADDR_SIZE], char tpa[IPV4_ADDR_SIZE]) {
    void *packet = malloc(PACKET_MAX_SIZE);
    int packet_len = 0;

    struct arp arp = {
        .htype = ARP_HTYPE_ETH,
        .ptype = PTYPE_IPV4,

        .hlen = ARP_HLEN_ETH,
        .plen = ARP_PLEN_IPV4,
        .oper = oper,
    };

    // Addresses
    memcpy(arp.sha, sha, MAC_ADDR_SIZE);
    memcpy(arp.spa, spa, IPV4_ADDR_SIZE);
    memcpy(arp.tha, tha, MAC_ADDR_SIZE);
    memcpy(arp.tpa, tpa, IPV4_ADDR_SIZE);

    packet_len += sizeof(struct arp);
    memcpy(packet, &arp, packet_len);

    // Ethernet encapsulation
    encapsulate_ethernet(packet, &packet_len, sha, g_nic.mac, PTYPE_ARP);

    send_packet(packet, packet_len);

    free(packet);
}
