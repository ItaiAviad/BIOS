// PIT - Programmable Interval Timer

#pragma once
#ifndef PIT_H
#define PIT_H

#include <stdio.h>
#include <stdlib.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/pic.h>

unsigned read_pit_count(void);

#endif
