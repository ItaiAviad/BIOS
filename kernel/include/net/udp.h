/* net/udp.h -- declarations for inquiring about the UDP Protocol
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols
         https://en.wikipedia.org/wiki/User_Datagram_Protocol#UDP_datagram_structure */

#pragma once
#ifndef _NET_UDP_H
#define _NET_UDP_H	1

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <net/ipv4.h>


/* UDP Header */

// Source Port: 16 bits

// Destination Port: 16 bits

// Length: 16 bits

// Checksum: 16 bits

// Data: Variable

__attribute__((packed)) struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;

    uint16_t length;
    uint16_t checksum;
};

void *encapsulate_udp(void *packet, int *lenp, int src_port, int dst_port);

#endif
