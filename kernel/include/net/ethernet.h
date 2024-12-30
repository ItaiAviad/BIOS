/* net/ethernet.h -- declarations for inquiring about the Ethernet Frame
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols */

#pragma once
#ifndef _NET_ETHERNET_H
#define _NET_ETHERNET_H	1

#include <types.h>
#include <stdio.h>

#define MAC_ADDR_SIZE 6
__attribute__((packed)) struct mac {
    uint8_t octets[MAC_ADDR_SIZE];
};

__attribute__((packed)) struct ethernet {
    struct mac mac_dst;
    struct mac mac_src;
    uint16_t type; // can also be length
};

void print_mac(struct mac mac);
void mac_reverse_type_endian(struct ethernet *eth);

#endif
