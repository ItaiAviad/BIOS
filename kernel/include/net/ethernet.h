/* net/ethernet.h -- declarations for inquiring about the Ethernet Frame
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols */

#pragma once
#ifndef _NET_ETHERNET_H
#define _NET_ETHERNET_H	1

#include <types.h>
#include <stdio.h>

#define ETH_FRAME_MIN_SIZE 64

#define MAC_ADDR_SIZE 6

__attribute__((packed)) struct ethernet {
    char hwdst[MAC_ADDR_SIZE]; // Destination MAC Address
    char hwsrc[MAC_ADDR_SIZE]; // Source MAC Address
    uint16_t type; // can also be length
};

void print_mac(char mac[MAC_ADDR_SIZE]);
uint16_t ethernet_type_reverse(uint16_t type);

void *encapsulate_ethernet(void *packet, int *lenp, char hwdst[MAC_ADDR_SIZE], char hwsrc[MAC_ADDR_SIZE], uint16_t type);


/* EtherType */
#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_ARP 0x0806
#define ETHERTYPE_IPV6 0x86DD

#endif
