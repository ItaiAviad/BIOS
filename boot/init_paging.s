
; Initialize the page table
; 
; The Page Table has 4 components which will be mapped as follows:
;
; PML4T -> 0x1000 (Page Map Level 4 Table)
; PDPT  -> 0x2000 (Page Directory Pointer Table)
; PDT   -> 0x3000 (Page Directory Table)
; PT    -> 0x4000 (Page table)


[bits 32]

init_paging:
    pushad

    ; Clear the memory area using rep stosd
    mov edi, 0x1000
    mov cr3, edi            ; Save the PML4T start address in cr3. This will save us time later because cr3 is what the CPU uses to locate the page table entries
    xor eax, eax
    mov ecx, 4096
    rep stosd
    ; Set edi back to PML4T[0]
    mov edi, cr3

    ; Set up the first entry of each table
    ;
    ; This part can be a little confusing, and it took me a while to understand.
    ; The key is knowing that the page tables MUST be page aligned. This means
    ; the lower 12 bits of the physical address (3 hex digits) MUST be 0. Then,
    ; each page table entry can use the lower 12 bits as flags for that entry.
    ;
    ; You may notice that we're setting our flags to "0x003", because we care most
    ; about bits 0 and 1. Bit 0 is the "exists" bit, and is only set if the entry
    ; corresponds to another page table (for the PML4T, PDPT, and PDT) or a page of
    ; physical memory (in the PT). Obviously we want to set this. Bit 1 is the
    ; "read/write" bit, which allows us to view and modify the given entry. Since we
    ; want our OS to have full control, we'll set this as well.
    ;
    ; Now let's wire up our table. Note that edi is already at PML4T[0]
    mov dword[edi], 0x2003      ; Set PML4T[0] to address 0x2000 (PDPT) with flags 0x0003
    add edi, 0x1000             ; Go to PDPT[0]
    mov dword[edi], 0x3003      ; Set PDPT[0] to address 0x3000 (PDT) with flags 0x0003
    add edi, 0x1000             ; Go to PDT[0]
    mov dword[edi], 0x4003      ; Set PDT[0] to address 0x4000 (PT) with flags 0x00000003

    
    add edi, 0x1000             ; Go to PT[0]
    mov ebx, 0x00000003         ; EBX has address 0x0000 with flags 0x0003
    mov ecx, 512                ; Do the operation 512 times

    add_page_entry_protected:
        ; a = address, x = index of page table, flags are entry flags
        mov dword[edi], ebx                 ; Write ebx to PT[x] = a.append(flags)
        add ebx, 0x1000                     ; Increment address of ebx (a+1)
        add edi, 8                          ; Increment page table location (since entries are 8 bytes)
                                            ; x++
        loop add_page_entry_protected       ; Decrement ecx and loop again

    ; Set up PAE paging, but don't enable it quite yet
    ;
    ; Here we're basically telling the CPU that we want to use paging, but not quite yet.
    ; We're enabling the feature, but not using it.
    mov eax, cr4
    or eax, 1 << 5               ; Set the PAE-bit, which is the 5th bit
    mov cr4, eax

    ; Now we should have a page table that identities maps the lowest 2MB of physical memory into virtual memory
    popad
    ret
