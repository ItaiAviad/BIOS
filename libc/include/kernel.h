// Kernel related functions and definitions
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
    
    bool up;
    bool down;
    bool left;
    bool right;
} keyboard_state;

void shutdown(void);

void tty_init(void);

void ursp(uint64_t rsp);

#endif
