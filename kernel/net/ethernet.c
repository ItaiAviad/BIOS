#include <net/ethernet.h>

void print_mac(struct mac mac) {
    printf("%x:%x:%x:%x:%x:%x", mac.octets[0], mac.octets[1], mac.octets[2], mac.octets[3], mac.octets[4], mac.octets[5]);
}

void mac_reverse_type_endian(struct ethernet *eth) {
    eth->type = (eth->type >> 8) | (eth->type << 8);
}
