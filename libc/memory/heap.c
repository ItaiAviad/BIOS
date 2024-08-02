#include <memory.h>
#include <stdint.h>


PageFrameAllocator allocator = {.initialized = false};

void* init_heap(uint64_t base_addr, uint64_t size) {

    // Init Page Frame Allocator
    if (!allocator.initialized) {
        init_page_frame_allocator(&allocator, PAGE_SIZE * 8192);
        allocator.initialized = 1;
    }

    heap_base = (void*) aalign(base_addr, HEAP_CHUNK_MIN_SIZE_BYTES);

    // Allocate malloc_state (heap:0x0)- Kernel Malloc
    kmalloc((uint64_t*) PML4_KERNEL, &allocator, sizeof(malloc_state));

    malloc_state heap;
    heap.heap_base = heap_base;
    heap.heap_size = aalign(size, HEAP_CHUNK_MIN_SIZE_BYTES);
    heap.mchunk = NULL;
    heap.unsorted_bin_head = NULL;
    memcpy(heap_base, (void*) &heap, sizeof(malloc_state));

    brk(heap_base + sizeof(malloc_state));

    return (void*) heap_base;
}

int brk(void *end_data_segment) {
    heap_end = end_data_segment;
    return 0;
}

void* sbrk(size_t increment) {
    void* old_break = heap_end;
    heap_end += aalign(increment, HEAP_CHUNK_MIN_SIZE_BYTES);
    return (void*) old_break;
}

