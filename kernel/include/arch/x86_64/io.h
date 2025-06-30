#pragma once
#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stdbool.h>

#include <string.h>
#include <kernel.h>
#include <limits.h>

#include <arch/x86_64/pic.h>
#include <arch/x86_64/scs1.h>
#include <arch/x86_64/interrupts.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define LOG_SYM_SUC "[+]"
#define LOG_SYM_FAI "[-]"
#define LOG_SYM_INF "[*]"
#define LOG_SYM_DEBUG "[D]"
#define LOG_SYM_ERR "[!]"

typedef struct keyboard_t {
    char buffer[BUFFER_SIZE];
    char tmp[BUFFER_SIZE];

    size_t buffer_head;
    size_t buffer_tail;

    keyboard_state ks;
} keyboard_t;

/**
 * @brief Handle key press (including special key presses, e.g. LeftShift, CapsLock, RightShift)
 * 
 * @param unsigned_char 
 */
void buffer_put(unsigned char);
void buffer_put_c(unsigned char);
/**
 * @brief Get next char from buffer (or special character press, i.e. cursor keys)
 * 
 * @return int 
 */
int buffer_get(void);
/**
 * @brief true if buffer is empty and no special key was pressed
 * 
 * @return int 
 */
int buffer_is_empty(void);
/**
 * @brief Clear the buffer
 * 
 */
void buffer_clear(void);
int buffer_len(void);

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
 * @return char - ascii char read (or special key)
 */
int wait_key();

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
 * @brief Writes a word to a port
 * 
 * @param port 
 * @param val 
 */
static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ( "out %1, %0" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * @brief Reads a word from a port
 * 
 * @param port 
 * @return uint32_t 
 */
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile ( "in %0, %1"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

/**
 * @brief Writes a long-word to a port
 * 
 * @param port 
 * @param val 
 */
static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ( "out %1, %0" : : "a"(val), "Nd"(port) : "memory");
}

/**
 * @brief Reads a long-word from a port
 * 
 * @param port 
 * @return uint32_t 
 */
static inline uint32_t inl(uint16_t port)
{
    uint32_t value;
    __asm__ volatile (
        "in %0, %1"
        : "=a" (value)        // output: value will be stored in the EAX register
        : "Nd" (port)         // input: port is a 16-bit immediate value
    );
    return value;
}

/**
 * @brief Wait
 * 
 */
static inline void io_wait(void)
{
    for (int i = 0; i < 2; i++)
        outb(0x80, 0x0);
}

#endif
