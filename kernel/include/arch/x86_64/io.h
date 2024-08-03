#pragma once
#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <arch/x86_64/pic.h>
#include <arch/x86_64/scs1.h>
#include <arch/x86_64/interrupts.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2


// IO Buffer
#define BUFFER_SIZE 256

typedef struct keyboard_t {
    char buffer[BUFFER_SIZE];
    size_t buffer_head;
    size_t buffer_tail;

    bool caps;
    bool lshift;
    bool lctrl;
    bool lalt;
    bool altgr;
    bool rctrl;
    bool rshift;
} keyboard_t;

/**
 * @brief Handle key press (including special key presses, e.g. LeftShift, CapsLock, RightShift)
 * 
 * @param unsigned_char 
 */
void buffer_put(unsigned char);
void buffer_put_c(unsigned char);
unsigned char buffer_get(void);
int buffer_is_empty(void);

// Keyboard

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

// Inline Assembly
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
    for (int i = 0; i < 100; i++)
        outb(0x80, 0x0);
}
static inline void io_wait_fast(void)
{
    for (int i = 0; i < 1; i++) {
        __asm__ volatile ("nop");
        outb(0x80, 0x0);
    }
}

#endif
