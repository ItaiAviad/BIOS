// Memory Layout

#ifndef MLAYOUT_H

#define MLAYOUT_H

#include <types.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define KERNEL_HEAP_START 0x1000000
#define KERNEL_HEAP_SIZE_PAGES 4
extern uint64_t __kend;
extern uint64_t __entry;

#endif
