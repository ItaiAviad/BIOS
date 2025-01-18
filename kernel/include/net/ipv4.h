/* net/ipv4.h -- declarations for inquiring about the IPv4 Protocol
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols */

#pragma once
#ifndef _NET_IPV4_H
#define _NET_IPV4_H	1

#include <types.h>
#include <stdio.h>

#define IPV4_ADDR_SIZE 0x4

void print_ipv4(char ipv4[IPV4_ADDR_SIZE]);

#endif
