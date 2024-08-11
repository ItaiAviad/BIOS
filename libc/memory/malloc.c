#include <memory.h>
#include <string.h>
#include <arch/x86_64/mmu.h>

void* malloc(size_t size) {
    malloc_state* heap = (malloc_state*) heap_base;

    malloc_chunk new_mchunk;

    void* new_addr = (void*) 0x0;

    // Find a suitable free chunk
    bool found_free = false;
    malloc_chunk* cur = heap->unsorted_bin_head;
    while (cur != NULL)
    {
        if (cur->mchunk_size >= size) { // TODO: Split found chunk (if remainder > min_chunk_size)
            found_free = true;

            new_mchunk = *cur;

            // Unlink - Unsorted Bin Doubly Linked List
            if (cur->bk != NULL)
                cur->bk->fd = cur->fd;
            else
                heap->unsorted_bin_head = cur->fd;
            if (cur->fd != NULL)
                cur->fd->bk = cur->bk;
            // else
                // heap->unsorted_bin_tail = cur->bk;

            new_mchunk.mchunk_prev_size = 0x0;
            break;
        }
        cur = cur->fd;
    }
    
    if (!found_free) {
        // Expand the heap
        size_t data_size = max(HEAP_CHUNK_MIN_SIZE_BYTES - sizeof(malloc_chunk), aalign(size, 0x8));
        new_addr = sbrk(size); // Heap old end address

        new_mchunk.mchunk_prev_size = 0;
        new_mchunk.mchunk_size = sizeof(malloc_chunk) + data_size;
        new_mchunk.data = new_addr + sizeof(malloc_chunk);
    }

    // Link - Allocated Doubly Linked List
    new_mchunk.bk = NULL;
    new_mchunk.fd = heap->mchunk;
    if (new_mchunk.fd != NULL)
        new_mchunk.fd->bk = &new_mchunk;

    // Memory
    // Map page(s) if nescessary - Kernel Malloc
    kmalloc((uint64_t*) PML4_KERNEL, &allocator, new_mchunk.mchunk_size);
    
    // If new, memcpy header to new memory location
    if (!found_free)
        memcpy(new_addr, &new_mchunk, sizeof(malloc_chunk));

    // Clear data memory
    memset(new_mchunk.data, 0x0, (new_mchunk.mchunk_size - sizeof(malloc_chunk)));

    return new_mchunk.data;
}