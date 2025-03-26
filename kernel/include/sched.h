#pragma once
#ifndef SCHED_H
#define SCHED_H
#include <types.h>

#define PROCESS_RUN_TIME 3000// In ms


extern uint64_t last_context_switch_time;

#include <process.h>


void handle_sched_on_pit_tick(registers* registers, uint64_t cr3);


#endif
