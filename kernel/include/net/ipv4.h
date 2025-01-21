/* net/ipv4.h -- declarations for inquiring about the IPv4 Protocol
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols
         https://en.wikipedia.org/wiki/IPv4#Header */

#pragma once
#ifndef _NET_IPV4_H
#define _NET_IPV4_H	1

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>

/* IPv4 Header */

#define IPV4_ADDR_SIZE 0x4

// Version: 4bits
#define IPV4_VERSION_IPV4 0x4

// Internet Header Length (IHL): 4bits
// The number of 32-bit words in the header.
// Minimum value is 5 (5 * 32-bit words = 20 bytes)
// Maximum value is 15 (15 * 32-bit words = 60 bytes)

// Differentiated Services Code Point (DSCP): 6 bits

// Explicit Congestion Notification (ECN): 2 bits

// Total Length: 16 bits

// Identification: 16 bits

// Flags: 3 bits
// Reserved (R): 1 bit
// Don't Fragment (DF): 1 bit
// More Fragments (MF): 1 bit

// Fragment Offset: 13 bits

// Time to live (TTL): 8 bits
#define IPV4_TTL_DEFAULT 0x40

// Protocol: 8 bits
#define IPV4_PROTOCOL_ICMP 0x1
#define IPV4_PROTOCOL_TCP 0x6
#define IPV4_PROTOCOL_UDP 17

// Header Checksum: 16 bits

// Source address: 32 bits

// Destination address: 32 bits

// Options: 0 - 320 bits, padded to multiples of 32 bits

__attribute__((packed)) struct ipv4_header {
    uint8_t version_ihl;
    uint8_t dscp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    unsigned char src_addr[IPV4_ADDR_SIZE];
    unsigned char dst_addr[IPV4_ADDR_SIZE];
    // uint32_t options;
};

void print_ipv4(volatile unsigned char ipv4[IPV4_ADDR_SIZE]);

// Calculate the IPv4 header checksum
uint16_t ipv4_checksum(void *vdata, size_t length);

void *encapsulate_ipv4(void *packet, int *lenp, int protocol, unsigned char src_ip[IPV4_ADDR_SIZE], unsigned char dst_ip[IPV4_ADDR_SIZE]);

#endif
