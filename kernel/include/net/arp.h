/* net/arp.h -- declarations for inquiring about the ARP Frame
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols */

#pragma once
#ifndef _NET_ARP_H
#define _NET_ARP_H	1

#include <types.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <net/ethernet.h>
#include <net/ipv4.h>
#include <net/rtl8139.h>

// Hardware Type (HTYPE): 16bits
#define ARP_HTYPE_ETH 0x1

// Protocol Type (PTYPE): 16bits
// See: <net/ethernet.h>
#define PTYPE_IPV4 ETHERTYPE_IPV4
#define PTYPE_ARP ETHERTYPE_ARP
#define PTYPE_IPV6 ETHERTYPE_IPV6

// Hardware Length (HLEN): 8bits
#define ARP_HLEN_ETH 0x6

// Protocol Length (PLEN): 8bits
#define ARP_PLEN_IPV4 0x4

// Operation (OPER): 16bits
#define ARP_OPER_REQUEST 0x1
#define ARP_OPER_REPLY 0x2

// Sender Hardware Address (SHA): 48bits
/*
Media address of the sender.
In an ARP request this field is used to indicate the address of the host sending the request.
In an ARP reply this field is used to indicate the address of the host that the request was looking for.
*/

// Sender Protocol Address (SPA): 32bits
/*
Internetwork address of the sender.
*/

// Target Hardware Address (THA): 48bits
/*
Media address of the intended receiver. In an ARP request this field is ignored.
In an ARP reply this field is used to indicate the address of the host that originated the ARP request.
*/

// Target Protocol Address (TPA): 32bits
/*
Internetwork address of the intended receiver.
*/

__attribute__((packed)) struct arp_header {
    uint16_t htype;
    uint16_t ptype;

    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;

    unsigned char sha[MAC_ADDR_SIZE]; // Sender Hardware Address
    unsigned char spa[IPV4_ADDR_SIZE]; // Sender Protocol Address

    unsigned char tha[MAC_ADDR_SIZE]; // Target Hardware Address
    unsigned char tpa[IPV4_ADDR_SIZE]; // Target Protocol Address
};

#define ARP_WAIT_TIMEOUT 3000 // Timeout in milliseconds
#define ARP_CACHE_TTL 30000 // 30 seconds
#define ARP_CACHE_CLEANUP_INTERVAL 10000 // Check cache every 10 seconds

struct arp_entry {
    unsigned char ip[IPV4_ADDR_SIZE];
    unsigned char mac[MAC_ADDR_SIZE];
    uint64_t timestamp; // Time when the entry was added
};

void clear_arp_cache();

void cache_arp_entry(unsigned char ip[IPV4_ADDR_SIZE], unsigned char mac[MAC_ADDR_SIZE]);
unsigned char *find_mac_in_cache(unsigned char ip[IPV4_ADDR_SIZE]);

void send_arp(uint16_t oper, unsigned char sha[MAC_ADDR_SIZE], unsigned char spa[IPV4_ADDR_SIZE], unsigned char tha[MAC_ADDR_SIZE], unsigned char tpa[IPV4_ADDR_SIZE]);

/**
 * @brief Wait for ARP entry of `ip` to appear in ARP Cache
 * 
 * @param ip 
 * @return unsigned char* can be NULL
 */
unsigned char *wait_for_arp_entry(unsigned char ip[IPV4_ADDR_SIZE]);
/**
 * @brief Get MAC by IPv4
 * 
 * @param ip 
 * @return unsigned char* can be NULL
 */
unsigned char *get_mac(unsigned char ip[IPV4_ADDR_SIZE]);

/**
 * @brief Handle receive of ARP Request/Reply
 * 
 * @param pkt 
 */
void handle_packet_arp(struct packet *pkt);

#endif
