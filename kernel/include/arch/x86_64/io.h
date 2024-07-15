#pragma once
#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define PS2_KEYBOARD_PORT_DATA 0x60
#define PS2_KEYBOARD_PORT_STATUS_REG 0x64
#define PS2_KEYBOARD_PORT_CMD_REG 0x64
#define PS2_KEYBOARD_SCS1_RLS_IDX 0x81

/**
 * @brief Check if key in scs1 is pressed
 * 
 * @param scan_code 
 * @return true - key is pressed
 * @return false - key is not pressed
 */
bool scs1_is_press(uint16_t scan_code);
/**
 * @brief Handle special key press (e.g. LeftShift, CapsLock)
 * 
 * @param scan_code 
 */
void special_key_press(uint16_t scan_code);
/**
 * @brief Wait for key
 * 
 * @return char - ascii char read
 */
char wait_key();
/**
 * @brief Convert scs1 scancode to ascii
 * 
 * @param scan_code 
 * @return char - ascii value
 */
char scs1_to_ascii(uint16_t scan_code);

/**
 * @brief Writes a byte to a port
 * 
 * @param port 
 * @param val 
 */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %w1, %b0" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * @brief Reads a byte from a port
 * 
 * @param port 
 * @return uint8_t 
 */
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %b0, %w1"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

/**
 * @brief Wait
 * 
 */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

#endif
