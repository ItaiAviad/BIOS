#include <net/udp.h>

void *encapsulate_udp(void *packet, int *lenp, int src_port, int dst_port) {
    // increase length
    *lenp += sizeof(struct udp_header);

    struct udp_header udp_header = {
        .src_port = be16toh(src_port),
        .dst_port = be16toh(dst_port),

        .length = be16toh(*lenp),
        .checksum = be16toh(0x0),
    };

    // update packet
    memmove(packet + sizeof(struct udp_header), packet, *lenp);
    memcpy(packet, &udp_header, sizeof(struct udp_header));

    // Calculate checksum
    // udp_header.checksum = htobe16(udp_checksum(&udp_header, sizeof(struct udp_header)));

    return packet;
}
