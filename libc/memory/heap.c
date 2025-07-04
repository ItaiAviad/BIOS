#if defined(__is_libk)
#include <arch/x86_64/mmu.h>
#endif

#include <memory.h>
#include <sys/syscall.h>


void* init_heap(__attribute__((unused)) PCB pcb, uint64_t base_addr, uint64_t size, bool map) {
    g_heap_malloc_state_base = (void*) aalign(base_addr, HEAP_CHUNK_MIN_SIZE_BYTES);

    // Allocate malloc_state (heap:0x0)- Kernel Malloc
    // kmalloc(sizeof(malloc_state));
    // For easier handling of unwanted segmentation in Heap, allocate all heap memory
    // kmalloc(size); // TODO: Add kmalloc syscall

    malloc_state heap;
    heap.heap_base = g_heap_malloc_state_base;
    heap.heap_total_size = aalign(size, HEAP_CHUNK_MIN_SIZE_BYTES);
    heap.mchunk = NULL;
    heap.unsorted_bin_head = NULL;
    // TODO (IMPORTANT!): Use `kmalloc` (above) as a syscall
    if (map) {
    #if defined(__is_libk)
        map_memory_range(&pcb, g_heap_malloc_state_base, g_heap_malloc_state_base + sizeof(malloc_state), g_heap_malloc_state_base);
        map_memory_range(&pcb, (void*) base_addr, (void*) (base_addr + heap.heap_total_size), (void*) base_addr);
    #endif
    }
    memset(&heap.padding, 0x0, MALLOC_STATE_PADDING_SIZE);
    memcpy(g_heap_malloc_state_base, (void *)&heap, sizeof(malloc_state));

    brk(g_heap_malloc_state_base + sizeof(malloc_state));

    return (void *)g_heap_malloc_state_base;
}

int brk(void *end_data_segment) {
    malloc_state* heap = (malloc_state*) g_heap_malloc_state_base;
    heap->heap_end = end_data_segment;
    heap = (malloc_state*) g_heap_malloc_state_base;
    return 0;
}

void *sbrk(size_t increment) {
    malloc_state* heap = (malloc_state*) g_heap_malloc_state_base;
    void *old_break = heap->heap_end;
    heap->heap_end += aalign(increment, HEAP_CHUNK_MIN_SIZE_BYTES);
    return (void *)old_break;
}

void print_heap(void) {
#if defined(__is_libk)
    malloc_state *heap = (malloc_state *)g_heap_malloc_state_base;

    printf("heap_base: %p, tsize: %p, end: %p\n", heap->heap_base, heap->heap_total_size, heap->heap_end);

    printf("Allocated Chunks:\n");
    malloc_chunk *cur = heap->mchunk;
    while (cur != NULL) {
        printf("ptr: %p, size: %p, fd: %p, bk: %p, data: %p\n", cur, cur->mchunk_size, cur->fd, cur->bk, cur->data);
        cur = cur->fd;
    }

    printf("Unsorted Bin Chunks (Free):\n");
    cur = heap->unsorted_bin_head;
    while (cur != NULL) {
        printf("ptr: %p, size: %p, fd: %p, bk: %p, data: %p\n", cur, cur->mchunk_size, cur->fd, cur->bk, cur->data);
        cur = cur->fd;
    }

#else
    syscall(sys_print_heap);
#endif
}
