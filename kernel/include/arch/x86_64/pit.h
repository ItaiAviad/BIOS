// PIT - Programmable Interval Timer

#pragma once
#ifndef PIT_H
#define PIT_H

#include <stdio.h>
#include <stdlib.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/pic.h>

#define PIT_COMMAND_REGISTER 0x43
#define PIT_CHANNEL_0_DATA_REGISTER 0x40
#define PIT_FREQUENCY 1193180 // PIT frequency (Hz)
#define PIT_TIMER_FREQ 1000       // Desired frequency for PIT (1 kHz)
#define TIME_DELTA_TICK_MS (1000/PIT_TIMER_FREQ)

extern uint64_t pit_time_ms;

void pit_init(void);
uint64_t read_pit_count(void);
void set_pit_count(uint64_t count);

// PIT Timer
void pit_handler();
/**
 * @brief Sleep for a specified number of milliseconds
 * 
 * @param uint64_t delay (in milliseconds)
 */
void pit_sleep(uint64_t);

#endif
