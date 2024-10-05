#pragma once
#ifndef SCS1_H
#define SCS1_H

#include <stdbool.h>
#include <types.h>



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
bool scs1_is_press(uint16_t);

/**
 * @brief Convert scs1 scancode to ascii
 * 
 * @param uint16_t scan_code 
 * @param bool shift active
 * @param bool caps active
 * @return char - ascii value
 */
char scs1_to_ascii(uint16_t scancode, bool shift, bool caps);

#endif
