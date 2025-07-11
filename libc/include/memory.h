#pragma once
#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include <arch/x86_64/mmu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Address Align (bigger (>) than given address)
 * 
 * @param address
 * @param alignment
 * @return uint64_t 
 */
uint64_t aalign(uint64_t, uint64_t);
uint64_t aalign_down(uint64_t addr, uint64_t alignment);

// Heap
#define HEAP_CHUNK_MIN_SIZE_BYTES 64

// Chunk
typedef struct __attribute__((__packed__)) malloc_chunk {
    uint32_t mchunk_prev_size;  // Size of previous chunk, if it is free
    uint32_t mchunk_size;       // Size in bytes, including overhead
    struct malloc_chunk* fd;    // Double links - used only if this chunk is free
    struct malloc_chunk* bk;
    void* data;
} malloc_chunk;

// Heap State
#define MALLOC_STATE_PADDING_SIZE HEAP_CHUNK_MIN_SIZE_BYTES - (2 * sizeof(void*) + sizeof(size_t) + sizeof(malloc_chunk*) * 2)
typedef struct __attribute__((__packed__)) malloc_state {
    void* heap_base;
    void* heap_end;
    size_t heap_total_size;
    malloc_chunk* mchunk;
    malloc_chunk* unsorted_bin_head;
    char padding[MALLOC_STATE_PADDING_SIZE];
} malloc_state;
// ----------------------------------------------


/**
 * @brief Initialize the heap
 * 
 * @param ctx context
 * @param uint64_t Heap base address
 * @param uint64_t Heap size
 * @param bool map if to map the heap pages
 * @return void* 
 */
void* init_heap(PCB pcb, uint64_t, uint64_t, bool map);
/**
 * @brief Set the heap address
 * 
 * @param end_data_segment 
 * @return int 
 */
int brk(void *end_data_segment);
/**
 * @brief Increase the heap
 * 
 * @return void* Old Heap end address
 */
void* sbrk(size_t);
/**
 * @brief Allocate memory
 * 
 * @param size 
 * @return void* 
 */
void* malloc(size_t);
/**
 * @brief Free memory
 * 
 * @param ptr 
 */
void free(void*);

void print_heap(void);

#ifdef __cplusplus
}
#endif

#endif