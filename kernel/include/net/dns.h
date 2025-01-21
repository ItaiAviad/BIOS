/* net/dns.h -- declarations for inquiring about the DNS Protocol
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols
    https://en.wikipedia.org/wiki/Domain_Name_System */

#pragma once
#ifndef _NET_DNS_H
#define _NET_DNS_H	1

#include <types.h>
#include <stdio.h>

#include <net/ethernet.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/udp.h>

/**
 * @brief Send DNS request
 * 
 * @param name 
 * @return unsigned char* - ip address of fixed size (ipv4 - 4, ipv6 - 16)
 */
unsigned char *send_dns(char *name);

#endif
