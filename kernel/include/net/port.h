/* net/port.h -- declarations for inquiring about the Ports
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols
         https://wiki.osdev.org/Network_Stack
         https://en.wikipedia.org/wiki/Port_(computer_networking) */

#pragma once
#ifndef _NET_PORT_H
#define _NET_PORT_H	1

#include <types.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>


enum DEFAULT_PORTS {
    FTP_DATA_TRANSFER = 20,
    FTP_CMD_CTL = 21,
    SSH = 22,
    TELNET = 23,
    SMTP = 25,
    DNS = 53,
    DHCP = 67, // 67+68
    HTTP = 80,
    HTTPS = 443,
    DHCPV6 = 546, // 546+547
};

#endif
