/* net/rtl8139.h -- declarations for inquiring about the RTL8139
    See: https://wiki.osdev.org/RTL8139 */

#pragma once
#ifndef _NET_RTL8139_H
#define _NET_RTL8139_H	1

#include <pci.h>
#include <memory.h>
#include <arch/x86_64/pic.h>

struct nic {
    uint64_t* rx_buf;
    uint16_t ioaddr;
    uint8_t mac[6];
};
extern struct nic nic;

#define RTL8139_DEVICE_ID 0x8139

// Registers (Offset (from IO base), Size, Name)
// See: pci.h/.c
#define RTL8139_OFFSET_TRANSMIT_BUF_0 0x20
#define RTL8139_OFFSET_TRANSMIT_BUF_1 0x24
#define RTL8139_OFFSET_TRANSMIT_BUF_2 0x28
#define RTL8139_OFFSET_TRANSMIT_BUF_3 0x2C

#define RTL8139_OFFSET_TRANSMIT_CMD_0 0x10
#define RTL8139_OFFSET_TRANSMIT_CMD_1 0x14
#define RTL8139_OFFSET_TRANSMIT_CMD_2 0x18
#define RTL8139_OFFSET_TRANSMIT_CMD_3 0x1C

#define RTL8139_OFFSET_MAC05 0x0
#define RTL8139_OFFSET_MAR07 0x8
#define RTL8139_OFFSET_RBSTART 0x30
#define RTL8139_OFFSET_CMD 0x37
#define RTL8139_OFFSET_IMR 0x3C
#define RTL8139_OFFSET_ISR 0x3E
#define RTL8139_OFFSET_RCR 0x44

#define RTL8139_SIZE_MAC05 6
#define RTL8139_SIZE_MAR07 8
#define RTL8139_SIZE_RBSTART 4
#define RTL8139_SIZE_CMD 1
#define RTL8139_SIZE_IMR 2
#define RTL8139_SIZE_ISR 2

#define RTL8139_CONFIG_1 0x52

#define RTL8139_SOFTWARE_RESET 0x10

#define RTL8139_INTERRUPT_PIN 11
#define RTL8139_INTERRUPT_LINE 11

#define TOK 0b100
#define ROK 0b01

void rtl8139_init(void);
void rtl8139_read_mac(void);
void rtl8139_handler(uint8_t isr, uint64_t error, uint64_t irq);

#endif
