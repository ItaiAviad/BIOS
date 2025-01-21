/* net/dns.h -- declarations for inquiring about the DNS Protocol
    See: https://wiki.osdev.org/Network_Stack#Networking_protocols
    https://en.wikipedia.org/wiki/Domain_Name_System#DNS_message_format */

#pragma once
#ifndef _NET_DNS_H
#define _NET_DNS_H	1

#include <types.h>
#include <stdio.h>

#include <net/ethernet.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/udp.h>


/* DNS Header */

// Transaction ID: 16 bits

// Flags: 16 bits
// QR: 1 bit
#define DNS_QR_QUERY 0
#define DNS_QR_REPLY 1
// OPCODE: 4 bits
#define DNS_OPCODE_QUERY 0
#define DNS_OPCODE_IQUERY 1
#define DNS_OPCODE_STATUS 2
// AA: 1 bit
// TC: 1 bit
// RD: 1 bit
// RA: 1 bit
// Z: 1 bit; (Z) == 0
// AD: 1 bit
// CD: 1 bit
// RCODE: 4 bits
#define DNS_RCODE_NOERROR 0
#define DNS_RCODE_FORMERR 1
#define DNS_RCODE_SERVFAIL 2
#define DNS_RCODE_NXDOMAIN 3

// Number of Questions: 16 bits

// Number of Answers: 16 bits

// Number of Authority RRs: 16 bits

// Number of Additional RRs: 16 bits

// RR
// TYPE
#define DNS_RR_TYPE_A 1
// CLASS
#define DNS_RR_CLASS_IN 1

__attribute__((packed)) struct dns_header {
    uint16_t tid;
    uint16_t flags;
    uint16_t num_q;
    uint16_t num_ans;
    uint16_t num_auth;
    uint16_t num_add;
};

// NOTICE: DNS Question is variable length (NAME, TYPE, CLASS where NAME is variable length)
__attribute((packed)) struct dns_question {
    uint16_t qtype;
    uint16_t qclass;
};

/**
 * @brief Send DNS request
 * 
 * @param name 
 * @return unsigned char* - ip address of fixed size (ipv4 - 4, ipv6 - 16)
 */
unsigned char *send_dns(char *name);

#endif
