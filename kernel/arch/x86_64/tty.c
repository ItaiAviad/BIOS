#include <kernel/tty.h>
#include <kernel/io.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row; // y position
static size_t terminal_column; // x position
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(CURSOR_PORT_COMMAND, 0x0A);
	outb(CURSOR_PORT_DATA, (inb(CURSOR_PORT_DATA) & 0xC0) | cursor_start);

	outb(CURSOR_PORT_COMMAND, 0x0B);
	outb(CURSOR_PORT_DATA, (inb(CURSOR_PORT_DATA) & 0xE0) | cursor_end);
}

void disable_cursor()
{
	outb(CURSOR_PORT_COMMAND, 0x0A);
	outb(CURSOR_PORT_DATA, 0x20);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outb(CURSOR_PORT_COMMAND, 0x0F);
	outb(CURSOR_PORT_DATA, (uint8_t) (pos & 0xFF));
	outb(CURSOR_PORT_COMMAND, 0x0E);
	outb(CURSOR_PORT_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t get_cursor_position(void)
{
    uint16_t pos = 0;
    outb(CURSOR_PORT_COMMAND, 0x0F);
    pos |= inb(CURSOR_PORT_DATA);
    outb(CURSOR_PORT_COMMAND, 0x0E);
    pos |= ((uint16_t)inb(CURSOR_PORT_DATA)) << 8;
    return pos;
}

void terminal_initialize(void) {
    // Cursor Setup
    enable_cursor(0, 0);

    // Terminal Setup
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
    terminal_writestring(">");
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
    update_cursor(terminal_column, terminal_row + 1);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
