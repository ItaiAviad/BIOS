#include <arch/x86_64/mmu.h>
#include <memory.h>
#include <string.h>

void* malloc(size_t size) {
    // Get Heap
    malloc_state* heap = (malloc_state*) heap_malloc_state_base;

    malloc_chunk new_mchunk;
    size = aalign(size, 0x10); // Align to 16 bytes

    void *new_addr = (void *)0x0;

    // Find a suitable free chunk
    bool found_free = false;
    malloc_chunk *cur = heap->unsorted_bin_head;
    while (cur != NULL)
    {
        if (cur->mchunk_size >= size + sizeof(malloc_chunk)) { // TODO: Split found chunk (if remainder > min_chunk_size)
            // Found a suitable free chunk
            found_free = true;

            new_mchunk = *cur;
            new_addr = cur;

            // Unlink - Unsorted Bin Doubly Linked List
            if (cur->bk != NULL)
                cur->bk->fd = cur->fd;
            else
                heap->unsorted_bin_head = cur->fd;
            if (cur->fd != NULL)
                cur->fd->bk = cur->bk;

            new_mchunk.mchunk_prev_size = 0x0;
            break;
        }
        cur = cur->fd;
    }

    if (!found_free) {
        // Expand the heap
        size_t data_size = max(HEAP_CHUNK_MIN_SIZE_BYTES - sizeof(malloc_chunk), size);
        new_mchunk.mchunk_prev_size = 0;
        new_mchunk.mchunk_size = sizeof(malloc_chunk) + data_size;

        // Warning! Allocating after page end might lead to "segmentation fault"
        new_addr = sbrk(new_mchunk.mchunk_size); // Heap old end address

        new_mchunk.data = new_addr + sizeof(malloc_chunk);

        map_memory_range(k_ctx, new_addr, new_mchunk.data + data_size - 1, new_addr);
    }

    // Link - Allocated Doubly Linked List (Put new chunk at start of linked list)
    new_mchunk.bk = NULL;
    new_mchunk.fd = heap->mchunk;
    if (new_mchunk.fd != NULL)
        new_mchunk.fd->bk = new_addr;
    // Update Heap State (new chunk)
    heap->mchunk = new_addr;

    // Memory
    // If new, allocate memory (if needed) memcpy header to new memory location
    if (!found_free) {
        // Map page(s) if nescessary - Kernel Malloc
        kmalloc(new_mchunk.mchunk_size);

        // Paste malloc chunk data to memory
        memcpy(new_addr, &new_mchunk, sizeof(malloc_chunk));
    }

    // Clear data memory
    memset(new_mchunk.data, 0x0, (new_mchunk.mchunk_size - sizeof(malloc_chunk)));
    
    ((malloc_chunk* )new_addr)->fd = new_mchunk.fd;

    #ifdef DEBUG
    printf("%s MALLOC: data: %x, new_addr: %x, new_mchunk.fd: %x\n", DEBUG, new_mchunk.data, new_addr, new_mchunk.fd);
    #endif

    ((malloc_chunk *)new_addr)->fd = new_mchunk.fd;

#ifdef DEBUG
    printf("%s MALLOC: data: %d, new_addr: %d, new_mchunk.fd: %d\n", DEBUG, new_mchunk.data, new_addr, new_mchunk.fd);
#endif

    return new_mchunk.data;
}