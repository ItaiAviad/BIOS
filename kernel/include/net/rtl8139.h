/* net/rtl8139.h -- declarations for inquiring about the RTL8139
    See: https://wiki.osdev.org/RTL8139
         https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139D_DataSheet.pdf
         https://www.cs.usfca.edu/~cruse/cs326f04/RTL8139_ProgrammersGuide.pdf */

#pragma once
#ifndef _NET_RTL8139_H
#define _NET_RTL8139_H	1

#include <pci.h>
#include <memory.h>
#include <time.h>
#include <arch/x86_64/pic.h>

#include <net/ethernet.h>
#include <net/arp.h>
#include <net/ipv4.h>

#define RTL8139_DEVICE_ID 0x8139

// Registers (Offset (from IO base), Size, Name)
// See: pci.h/.c
#define RTL8139_OFFSET_MAC0 0x0
#define RTL8139_OFFSET_MAR0 0x8

#define RTL8139_OFFSET_TRANSMIT_CMD_0 0x10
#define RTL8139_OFFSET_TRANSMIT_CMD_1 0x14
#define RTL8139_OFFSET_TRANSMIT_CMD_2 0x18
#define RTL8139_OFFSET_TRANSMIT_CMD_3 0x1C

#define RTL8139_OFFSET_TRANSMIT_BUF_0 0x20
#define RTL8139_OFFSET_TRANSMIT_BUF_1 0x24
#define RTL8139_OFFSET_TRANSMIT_BUF_2 0x28
#define RTL8139_OFFSET_TRANSMIT_BUF_3 0x2C

#define RTL8139_OFFSET_TRANSMIT_OK 15

#define RTL8139_OFFSET_RBSTART 0x30
#define RTL8139_OFFSET_CMD 0x37
#define RTL8139_OFFSET_CAPR 0x38   // Current Address of Packet Read
#define RTL8139_OFFSET_CBR 0x3A
#define RTL8139_OFFSET_IMR 0x3C
#define RTL8139_OFFSET_ISR 0x3E
#define RTL8139_OFFSET_RCR 0x44

#define RTL8139_OFFSET_CONFIG_1 0x52

#define RTL8139_SOFTWARE_RESET 0x10

#define RTL8139_INTERRUPT_PIN 11
#define RTL8139_INTERRUPT_LINE 11

#define RX_BUFFER_SIZE (8*KB)  // 8KB
#define RX_BUFFER_LEN (RX_BUFFER_SIZE + 4*1516) // + 16 for wrap + max packet size

#define TOK 0b100
#define ROK 0b01

#define RTL8139_ALLOW_ROK_TOK (TOK + ROK)

#define AAP 0b1
#define APM 0b10
#define AM 0b100
#define AB 0b1000
#define RTL8139_RCR_ACCEPT_ALL (AAP + APM + AM + AB) // Note: There are more than 4 accept bits

#define RTL8139_RX_CRC_LEN 4

#define RTL8139_TRANSMIT_REGISTER_COUNT 4
#define RTL8139_TX_BUFFER_LEN_MAX 1792
#define PACKET_MAX_SIZE 1500

struct nic {
    uint32_t* rx_buf;
    uint16_t ioaddr;
    int trrc; // transmit round robin counter (0-3) (current transmit register)

    unsigned char mac[MAC_ADDR_SIZE];
    unsigned char ipv4[IPV4_ADDR_SIZE];
};
extern struct nic g_nic;


struct packet {
    uint8_t *header;
    int16_t len;
    uint8_t *data;
};

void rtl8139_init(void);
void rtl8139_read_self_mac(void);

void send_packet(void *packet, int packet_len);

void rtl8139_handler(uint8_t isr, uint64_t error, uint64_t irq);

#endif
