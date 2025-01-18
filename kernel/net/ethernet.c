#include <net/ethernet.h>
#include <string.h>

void print_mac(char mac[MAC_ADDR_SIZE]) {
    printf("%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

uint16_t ethernet_type_reverse(uint16_t type) {
    return (type >> 8) | (type << 8);
}

void *encapsulate_ethernet(void *packet, int *lenp, char hwdst[MAC_ADDR_SIZE], char hwsrc[MAC_ADDR_SIZE], uint16_t type) {
    struct ethernet eth = {
        .type = ethernet_type_reverse(type)
    };
    memcpy(eth.hwdst, hwdst, MAC_ADDR_SIZE);
    memcpy(eth.hwsrc, hwsrc, MAC_ADDR_SIZE);

    memmove(packet + sizeof(struct ethernet), packet, *lenp);
    memcpy(packet, &eth, sizeof(struct ethernet));

    *lenp += sizeof(struct ethernet);

    // zero out the padding
    memset(packet + *lenp, 0x0, ETH_FRAME_MIN_SIZE);

    // padding
    *lenp = (*lenp < ETH_FRAME_MIN_SIZE) ? ETH_FRAME_MIN_SIZE : *lenp;

    return packet;
}
