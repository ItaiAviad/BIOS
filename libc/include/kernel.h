// Kernel related functions and definitions
// Libc wrappers for Kernel functions
#pragma once
#ifndef _KERNEL_H
#define _KERNEL_H 1

#include <sys/cdefs.h>
#include <stdbool.h>

typedef struct {
    bool caps;
    bool lshift;
    bool lctrl;
    bool lalt;
    bool altgr;
    bool rctrl;
    bool rshift;

    bool ext; // Extended bytes (0xE0)
    
    bool up;
    bool down;
    bool left;
    bool right;
} keyboard_state;


#define CAN 0x18
// Extended Keycodes
enum KEYCODES_EXT {
    KEYCODES_EXT_START = 0x100,
    CURSOR_UP = 0x100,
    CURSOR_LEFT,
    CURSOR_RIGHT,
    CURSOR_DOWN,
};
void keycode_ext_handler(int c);

void shutdown(void);

void tty_init(void);

/**
 * @brief Syscall wrapper to clear stdin buffer
 * 
 */
void stdin_clear();
/**
 * @brief Insert string to stdin buffer
 * 
 * @param s 
 */
void stdin_insert(const char* s);

/**
 * @brief Send a DNS Request
 * 
 * @param s 
 */
void send_dns_request(const char* s);

#endif
