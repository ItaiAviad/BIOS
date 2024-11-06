#include <arch/x86_64/mmu.h>
#include <memory.h>


void* init_heap(uint64_t base_addr, uint64_t size) {
    heap_malloc_state_base = (void*) aalign(base_addr, HEAP_CHUNK_MIN_SIZE_BYTES);

    // Allocate malloc_state (heap:0x0)- Kernel Malloc
    // kmalloc(sizeof(malloc_state));
    // For easier handling of unwanted segmentation in Heap, allocate all heap memory
    // kmalloc(size); // TODO: Add kmalloc syscall

    malloc_state heap;
    heap.heap_base = heap_malloc_state_base;
    heap.heap_total_size = aalign(size, HEAP_CHUNK_MIN_SIZE_BYTES);
    heap.mchunk = NULL;
    heap.unsorted_bin_head = NULL;
    map_memory_range(k_ctx, heap_malloc_state_base, heap_malloc_state_base + sizeof(malloc_state), heap_malloc_state_base);
    map_memory_range(k_ctx, (void*) base_addr, (void*) (base_addr + heap.heap_total_size), (void*) base_addr);
    set_pml4_address(k_ctx.pml4);
    memset(&heap.padding, 0x0, MALLOC_STATE_PADDING_SIZE);
    memcpy(heap_malloc_state_base, (void *)&heap, sizeof(malloc_state));

    brk(heap_malloc_state_base + sizeof(malloc_state));

    return (void *)heap_malloc_state_base;
}

int brk(void *end_data_segment) {
    heap_end = end_data_segment;
    return 0;
}

void *sbrk(size_t increment) {
    void *old_break = heap_end;
    heap_end += aalign(increment, HEAP_CHUNK_MIN_SIZE_BYTES);
    return (void *)old_break;
}

void print_heap(void) {
    malloc_state *heap = (malloc_state *)heap_malloc_state_base;

    printf("heap_base: %d, tsize: %d, end: %d\n", heap->heap_base, heap->heap_total_size, heap_end);

    printf("Allocated Chunks:\n");
    malloc_chunk *cur = heap->mchunk;
    while (cur != NULL) {
        printf("ptr: %d, size: %d, fd: %d, bk: %d, data: %d\n", cur, cur->mchunk_size, cur->fd, cur->bk, cur->data);
        cur = cur->fd;
    }

    printf("Unsorted Bin Chunks (Free):\n");
    cur = heap->unsorted_bin_head;
    while (cur != NULL) {
        printf("ptr: %d, size: %d, fd: %d, bk: %d, data: %d\n", cur, cur->mchunk_size, cur->fd, cur->bk, cur->data);
        cur = cur->fd;
    }
}
