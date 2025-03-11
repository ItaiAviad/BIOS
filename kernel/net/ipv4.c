#include <net/ipv4.h>

void print_ipv4(volatile unsigned char ipv4[IPV4_ADDR_SIZE]) {
    printf("%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
}

// Calculate the IPv4 header checksum
uint16_t ipv4_checksum(void *vdata, size_t length) {
    uint16_t *data = (uint16_t *)vdata;
    uint32_t sum = 0;

    // Sum up 16-bit words
    for (size_t i = 0; i < length / 2; i++) {
        sum += data[i];
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    // If there's an odd byte, add it
    if (length % 2) {
        sum += *((uint8_t *)data + length - 1);
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
    }

    // One's complement
    return ~sum;
}

void *encapsulate_ipv4(void *packet, int *lenp, int protocol, unsigned char src_ip[IPV4_ADDR_SIZE], unsigned char dst_ip[IPV4_ADDR_SIZE]) {
    struct ipv4_header ipv4_header = {
        .version_ihl = IPV4_VERSION_IPV4 << 4 | sizeof(struct ipv4_header) / 4,
        .dscp_ecn = 0x0,
        .total_length = htobe16(sizeof(struct ipv4_header) + *lenp),
        .identification = htobe16(0x0),
        .flags_fragment_offset = htobe16(0b010 << 13 | 0x0),
        .ttl = IPV4_TTL_DEFAULT,
        .protocol = protocol,
        .header_checksum = htobe16(0x0) // temporary zero,
    };
    memcpy(ipv4_header.src_addr, src_ip, IPV4_ADDR_SIZE);
    memcpy(ipv4_header.dst_addr, dst_ip, IPV4_ADDR_SIZE);

    // Calculate checksum
    ipv4_header.header_checksum = htobe16(ipv4_checksum(&ipv4_header, sizeof(struct ipv4_header)));

    // update packet
    memmove(packet + sizeof(struct ipv4_header), packet, *lenp);
    memcpy(packet, &ipv4_header, sizeof(struct ipv4_header));

    // increase length
    *lenp += sizeof(struct ipv4_header);

    return packet;
}
