#pragma once
#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define HEAP_CHUNK_MIN_SIZE_BYTES 40

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((unused))
void* heap_base;
__attribute__((unused))
void* heap_end;

typedef struct __attribute__((__packed__)) malloc_chunk {
    uint32_t mchunk_prev_size;  /* Size of previous chunk, if it is free. */
    uint32_t mchunk_size;       /* Size in bytes, including overhead. */
    struct malloc_chunk* fd;    /* double links -- used only if this chunk is free. */
    struct malloc_chunk* bk;
    void* data;
} malloc_chunk;

typedef struct __attribute__((__packed__)) malloc_state {
    void* heap_base;
    size_t heap_size;

    malloc_chunk* mchunk;

    malloc_chunk* unsorted_bin_head;
    // malloc_chunk* unsorted_bin_tail;

    char padding[HEAP_CHUNK_MIN_SIZE_BYTES - (sizeof(uint64_t) + sizeof(size_t) + sizeof(malloc_chunk*) * 2)];
} malloc_state;

/**
 * @brief Initialize the heap
 * 
 * @param uint64_t Heap base address
 * @param uint64_t Heap size
 * @return void* 
 */
void* init_heap(uint64_t, uint64_t);
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
 * @return void* 
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
/**
 * @brief Address Align
 * 
 * @return uint64_t 
 */
uint64_t aalign(uint64_t, uint64_t);

#ifdef __cplusplus
}
#endif

#endif
