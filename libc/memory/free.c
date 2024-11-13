#include <memory.h>
#include <sys/syscall.h>

void free(void* ptr) {
#if defined(__is_libk)
    // Get Heap
    malloc_state* heap = (malloc_state*) heap_malloc_state_base;

    malloc_chunk* cur = heap->mchunk;
    while (cur != NULL)
    {
        // Find the malloc chunk to free
        if (cur->data == (malloc_chunk*) ptr) {
            // Unlink - Unsorted Bin Doubly Linked List
            if (cur->bk != NULL)
                cur->bk->fd = cur->fd;
            else
                heap->mchunk = cur->fd;
            if (cur->fd != NULL)
                cur->fd->bk = cur->bk;

            // Link in unsorted bins - Allocated Doubly Linked List (Put chunk at start of linked list)
            cur->bk = NULL;
            cur->fd = heap->unsorted_bin_head;
            if (cur->fd != NULL)
                cur->fd->bk = cur;
            // Update Heap State (to freed chunk)
            heap->unsorted_bin_head = cur;

            // Memory
            // Unmap page(s) if nescessary - Kernel Free
            // kfree(new_mchunk.mchunk_size);

            #ifdef DEBUG
            printf("%s FREE: cur: %d, cur->data: %d\n", DEBUG, cur, cur->data);
            #endif

            // Clear data memory
            memset(cur->data, 0x0, (cur->mchunk_size - sizeof(malloc_chunk)));

            return;
        }
        cur = cur->fd;
    }
#else
    syscall(sys_free, ptr);
#endif
}
