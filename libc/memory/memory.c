#include <memory.h>

uint64_t aalign(uint64_t addr, uint64_t alignment) {
    // Align addr by alignment
    if (addr % alignment == 0)
        return addr;
    return ((addr / alignment + 1) * alignment);
}

void* init_heap(uint64_t base_addr, uint64_t size) {
    heap_base = (void*) aalign(base_addr, HEAP_CHUNK_MIN_SIZE_BYTES);

    malloc_state* heap;
    memset(heap, 0, sizeof(malloc_state));
    printf("------\n");
    for (uint64_t i = 0; i < sizeof(malloc_state); i++)
    {
        printf("%d ", *((char*)heap));
    }
    printf("-----\n");
    heap->heap_base = heap_base;
    heap->heap_size = aalign(size, HEAP_CHUNK_MIN_SIZE_BYTES);
    heap->mchunk = NULL;
    heap->unsorted_bin_head = NULL;
    printf("not heap: %d\n", heap->heap_base);
    printf("not size: %d\n", heap->heap_size);
    printf("not mchunk: %d\n", heap->mchunk);
    printf("not bin: %d\n", heap->unsorted_bin_head);
    memcpy(heap_base, (void*) heap, sizeof(malloc_state));
    printf("123123123\n");
    for (uint64_t i = 0; i < sizeof(malloc_state); i++)
    {
        printf("%d ", *((char*)heap_base));
    }
    printf("ss\n");
    // heap->unsorted_bin_tail = NULL;

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

