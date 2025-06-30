#pragma once
#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdbool.h>


#include <string.h>

#define CURSOR_PORT_COMMAND 0x3D4
#define CURSOR_PORT_DATA 0x3D5

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
void update_cursor_absolute(uint16_t pos);
uint16_t get_cursor_position(void);

void advance_cursor();
void reverse_cursor();

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_scroll_line_down();

#endif
