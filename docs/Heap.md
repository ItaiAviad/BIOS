# Heap
How does our self implemented Heap work?

---

### Initialization - `init_heap()`
For "segmentation" prevention reasons, the Heap gets allocated with its entire size at the initialization phase.
<br>
Explanation: If we `malloc` more space than the current page has to offer, and the next free page is *not* right after the current page, segmentation will (probably) occur.

---

### `malloc_state`
```c
#define MALLOC_STATE_PADDING_SIZE HEAP_CHUNK_MIN_SIZE_BYTES - (sizeof(uint64_t) + sizeof(size_t) + sizeof(malloc_chunk*) * 2)
typedef struct __attribute__((__packed__)) malloc_state {
    void* heap_base;
    size_t heap_total_size;
    malloc_chunk* mchunk;
    malloc_chunk* unsorted_bin_head;
    char padding[MALLOC_STATE_PADDING_SIZE];
} malloc_state;
```

`malloc_state` is a struct that describes the current Heap. It includes:

* `heap_base` - Heap's base address (virtual address)

* `heap_total_size` - Heap's total size (allocated at the initialization phase)

* `mchunk` (linked list) - A linked list containing the **allocated chunks** (of type `malloc_chunk`) of the Heap.

* `unsorted_bin_head` (linked list) - A linked list containing the **free chunks** (of type `malloc_chunk`) of the Heap.

---

### `malloc_chunk`
```c
typedef struct __attribute__((__packed__)) malloc_chunk {
    uint32_t mchunk_prev_size;  // Size of previous chunk, if it is free
    uint32_t mchunk_size;       // Size in bytes, including overhead
    struct malloc_chunk* fd;    // Double links - used only if this chunk is free
    struct malloc_chunk* bk;
    void* data;                 // Data
} malloc_chunk;
```

---

### `void* malloc(size_t size)`
#### Allocate a chunk of memory on the Heap

* The `malloc` first searches for a **free chunk** in the `unsorted_bin_head` linked list of the `malloc_state` (which is at least the size of `size_t size`).

* The searching for a free chunk is *"first fit"*, meaning it takes the first free chunk which accommodates its needs.

* If a suitable free chunk was found, it is *unlinked* from the `unsorted_bin_head` linked list (of 'malloc_state`);

* If no (suitable) free chunk was found, a new chunk of memory is then allocated on the Heap, and the `heap_end` variable is moved back.

The new/free chunk is then *linked* at the head `mchunk` linked list (of `malloc_state`) (which means it's now the first node of the linked list).
<br><br>
Return: `void* data` of the allocated `malloc_chunk`.

---

### `void free(void* ptr)`
#### Free a (allocated) chunk of memory
Input: `void* ptr` - `void*` of a certain `void* data` (return value of `malloc`).
<br><br>

* `free` loops through the `unsorted_bin_head` (of `malloc_state`) to find the matching `void* data`  the the given `void* ptr`.

* If no chunk was found, `free` exists.

* If the chunk was found, it is *unlinked* from `mchunk` linked list, and *linked* to `unsorted_bin_head` linked list.
