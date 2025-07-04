// PIT - Programmable Interval Timer
// See: https://wiki.osdev.org/Programmable_Interval_Timer

#include <arch/x86_64/pit.h>

// PIT Timer Counter
volatile uint64_t pit_tick_count = 0;
volatile uint64_t pit_time = 0;

uint64_t pit_time_ms = 0;

void pit_init(void) {
    cli();

    uint64_t divisor = PIT_FREQUENCY / PIT_TIMER_FREQ;

    // Set PIT mode
    // Select channel (bits 6-7):       00  Channel 0
    // Access mode (bits 4-5):          11
    // Operating mode (bits 0-3):       011 
    // BCD/Binary mode (bit 0):         0   
    // outb(PIT_COMMAND_REGISTER, 0x36); // 00110110

    // Set PIT divisor
    set_pit_count(divisor);

    sti();
}

uint64_t read_pit_count(void) {
    uint64_t pit_count = 0;
	
	// Disable interrupts
	cli();
	
	// al = channel in bits 6 and 7, remaining bits clear
	outb(PIT_COMMAND_REGISTER, 0b0000000);
	
	pit_count = inb(PIT_CHANNEL_0_DATA_REGISTER);              // Low byte
    io_wait();
	pit_count |= inb(PIT_CHANNEL_0_DATA_REGISTER) << 8;        // High byte

    sti();
	
	return pit_count;
}

void set_pit_count(uint64_t count) {
	// Disable interrupts
	cli();
	
	// Set low byte
	outb(PIT_CHANNEL_0_DATA_REGISTER, count & 0xFF);              // Low byte
    io_wait();
	outb(PIT_CHANNEL_0_DATA_REGISTER, (count & 0xFF00) >> 8);     // High byte

    sti();
}

void pit_handler() {
    pit_tick_count++;
    pit_time_ms += TIME_DELTA_TICK_MS;
}

void pit_sleep(uint64_t milliseconds) {
    sti();
    volatile uint64_t end_ticks = pit_tick_count + milliseconds;
    #ifdef DEBUG
    printf("%s: Start ticks: %d, End ticks: %d\n", DEBUG, pit_tick_count, end_ticks);
    #endif

    while (pit_tick_count < end_ticks) {
        // For bypassing compiler optimization (see: https://en.wikipedia.org/wiki/Compiler_optimization):
        io_wait();
        // Wait here until the specified number of ticks have passed

        // TODO:
        // Yield CPU to other processes or use an efficient wait
    }
}

