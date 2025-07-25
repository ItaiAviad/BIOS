#include <net/ethernet.h>
#include <string.h>

void print_mac(unsigned char mac[MAC_ADDR_SIZE]) {
    uint8_t *hw = (uint8_t *)mac;
    printf("%x:%x:%x:%x:%x:%x", hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
}

void *encapsulate_ethernet(void *packet, int *lenp, unsigned char hwdst[MAC_ADDR_SIZE], unsigned char hwsrc[MAC_ADDR_SIZE], uint16_t type) {
    struct ethernet_header eth_header = {
        .type = htobe16(type),
    };
    memcpy(eth_header.hwdst, hwdst, MAC_ADDR_SIZE);
    memcpy(eth_header.hwsrc, hwsrc, MAC_ADDR_SIZE);

    // update packet
    memmove(packet + sizeof(struct ethernet_header), packet, *lenp);
    memcpy(packet, &eth_header, sizeof(struct ethernet_header));

    // increase length
    *lenp += sizeof(struct ethernet_header);

    // zero out the padding
    memset(packet + *lenp, 0x0, ETH_FRAME_MIN_SIZE);

    // padding
    *lenp = (*lenp < ETH_FRAME_MIN_SIZE) ? ETH_FRAME_MIN_SIZE : *lenp;

    return packet;
}
