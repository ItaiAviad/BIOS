// #include <arch/x86_64/mmu.h>
// #include <memory.h>


// uint64_t kheap_current = KERNEL_HEAP_START;
// uint64_t kheap_end = KERNEL_HEAP_START;
// uint64_t kheap_current_size_left = 0; // No allocated pages in the beginning

// void *kmalloc(size_t size) {
//     uint64_t kheap_current_old = kheap_current;
//     uint64_t kheap_end_old = kheap_end;

//     // Check if there is enough space for size in current allocated page
//     if ((long long) kheap_current_size_left - (long long) size >= 0 && kheap_current_size_left > 0)
//     {
//         kheap_current_size_left -= size;
//         uint64_t allocation_addr = kheap_current;
//         kheap_current += size;
//         return (void *)allocation_addr;
//     }

//     size = aalign(size, PAGE_SIZE);
//     size_t num_pages = size / PAGE_SIZE; // Round up to the nearest page

//     // Allocate new page(s)
//     uint64_t allocation_addr = kheap_current;
//     for (size_t i = 0; i < num_pages; i++) {
//         void *page = allocate_page(kpcb.ctx);
//         if (page == NULL) {
//             #ifdef DEBUG
//             printf("%s: GOT NULL PAGE!\n", DEBUG);
//             #endif
//             return NULL; // Out of memory
//         }
//         // Map page at end of heap
//         map_page(kpcb.ctx, (void*) kheap_end, page, PAGE_MAP_FLAGS);
//         memset((void*) kheap_end, 0, PAGE_SIZE);
//         kheap_end += PAGE_SIZE;
//         kheap_current_size_left += PAGE_SIZE;
//     }
//     kheap_current_size_left -= size;

//     // If it's first allocation for Heap (Reserving memory for Heap)
//     if (kheap_current_old == kheap_end_old) {
//         kheap_current_size_left += size;
//         // Reserve 1 Page for Heap Metadata (malloc_state)
//         kheap_current_size_left -= PAGE_SIZE;
//     }

//     return (void *)allocation_addr;
// }

// void kfree(__attribute__((unused)) void* ptr) {
//     // Kernel Free - Optional (Heap can just stay the same without freeing)
// }
