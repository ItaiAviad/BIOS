// PIT - Programmable Interval Timer
// See: https://wiki.osdev.org/Programmable_Interval_Timer

#include <arch/x86_64/pit.h>

unsigned read_pit_count(void) {
	unsigned count = 0;
	
	// Disable interrupts
	cli();
	
	// al = channel in bits 6 and 7, remaining bits clear
	outb(0x43, 0b0000000);
	
	count = inb(0x40);              // Low byte
	count |= inb(0x40) << 8;        // High byte

    sti();
	
	return count;
}
