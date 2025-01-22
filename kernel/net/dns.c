#include <net/dns.h>
#include <net/rtl8139.h>
#include <random.h>

unsigned char *send_dns(char *name) {
    if (name == NULL || strlen(name) <= 0)
        printf("DNS: Invalid name server\n");
    
    // void *packet = malloc(PACKET_MAX_SIZE);
    unsigned char packet[PACKET_MAX_SIZE];
    int packet_len = 0;

    // Application Layer (DNS)
    // DNS Header
    struct dns_header dns_header = {
        .tid = htobe16(rand() % (pow(2, 16) - 1)),
        .flags = htobe16(
            (DNS_QR_QUERY << 15) | // QR
            ((DNS_OPCODE_QUERY) << 11) | // OPCODE
            (0 << 10) | // AA
            (0 << 9) | // TC
            (1 << 8) | // RD
            (0 << 7) | // RA
            (0 << 6) | // Z
            (0 << 5) | // AD
            (0 << 4) | // CD
            (DNS_RCODE_NOERROR) // RCODE
        ),
        .num_q = htobe16(1),
        .num_ans = 0,
        .num_auth = 0,
        .num_add = 0,
    };
    memcpy(packet, &dns_header, sizeof(struct dns_header));
    packet_len += sizeof(struct dns_header);

    // DNS Question
    // Split to labels (strtok)
    // Add each label length + label
    for (char *label = strtok(name, "."); label != NULL; label = strtok(NULL, ".")) {
        ((unsigned char *) packet)[packet_len] = strlen(label);
        packet_len++;
        memcpy(packet + packet_len, label, strlen(label));
        packet_len += strlen(label);
    }
    // Add null-terminator
    ((unsigned char *) packet)[packet_len] = 0x0;
    packet_len++;

    struct dns_question dns_question = {
        .qtype = htobe16(DNS_RR_TYPE_A),
        .qclass = htobe16(DNS_RR_CLASS_IN),
    };
    memcpy(packet + packet_len, &dns_question, sizeof(struct dns_question));
    packet_len += sizeof(struct dns_question);

    // Transport Layer (UDP)
    encapsulate_udp(packet, &packet_len, 1234, 53);

    // Network Layer (IPv4)
    unsigned char dst_ip[IPV4_ADDR_SIZE] = {10, 0, 0, 138}; // default gateway
    g_nic.ipv4[0] = 10;
    g_nic.ipv4[1] = 0;
    g_nic.ipv4[2] = 0;
    g_nic.ipv4[3] = 24;
    encapsulate_ipv4(packet, &packet_len, IPV4_PROTOCOL_UDP, g_nic.ipv4, dst_ip);

    // Data Link Layer (Ethernet)
    // Get destination MAC address (ARP)
    printf("REQUESTED DOMAIN NAME: %s\n", name);
    printf("ARP REQ: \n");
    unsigned char dst_ipv4[IPV4_ADDR_SIZE] = {10, 0, 0, 138};
    unsigned char *mac = get_mac(dst_ipv4);
    if (mac != NULL)
        print_mac(mac);
    else
        printf("MAC not found");
    printf("\n");
    unsigned char dst_mac[MAC_ADDR_SIZE] = {0x00, 0xb8, 0xc2, 0xbe, 0x03, 0x6d};

    encapsulate_ethernet(packet, &packet_len, dst_mac, g_nic.mac, PTYPE_IPV4);

    send_packet(packet, packet_len);

    // free(packet);

    return dst_mac;
}
