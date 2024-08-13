
; Page Tables Setup (64bit level 4) - Identity maps the first 2MB of memory

;  Paging tables locations:
;   PML4T -> 0x1000 (Page Map Level 4 Table)
;   PDPT  -> 0x2000 (Page Directory Pointer Table)
;   PDT   -> 0x3000 (Page Directory Table)
;   PT    -> 0x4000 (Page table)

; Source: https://wiki.osdev.org/Paging
; Source: https://wiki.osdev.org/Setting_Up_Paging
; Source: https://wiki.osdev.org/Identity_Paging
; Source: https://wiki.osdev.org/Setting_Up_Paging_With_PAE


[bits 32]

init_paging:
    pushad
    ; Clear the memory area using rep stosd
    mov edi, 0x1000
    mov cr3, edi ; Save PML4T start address in cr3
    xor eax, eax
    mov ecx, 4096
    rep stosd
    ; Set edi back to PML4T[0]
    mov edi, cr3

    ; Set table's pointers (and allocate 4MB of memory)
    mov edi, 0x1000
    mov dword[edi], 0x2003      ; Set PML4T[0] to address 0x2000 (PDPT), flags: 0x0003
    mov edi, 0x2000             ; Go to PDPT[0]
    mov dword[edi], 0x3003      ; Set PDPT[0] to address 0x3000 (PDT), flags: 0x0003
    mov edi, 0x3000             ; Go to PDT[0]
    mov dword[edi], 0x4003      ; Set PDT[0] to address 0x4000 (PT), flags: 0x0003
    mov edi, 0x3008
    mov dword[edi], 0x5003      ; Set PDT[1] to address 0x4000 (PT), flags: 0x0003
    mov edi, 0x3010
    mov dword[edi], 0x6003      ; Set PDT[2] to address 0x4000 (PT), flags: 0x0003

    mov edi, 0x4000             ; Go to PT[0] ; (0MB-2MB)
    mov ebx, 0x00000003         ; EBX has address 0x0000, flags: 0x0003
    mov ecx, 512                ; 512 times (512 entries in a table)

    .add_page_entry_protected:
        ; a = address, x = index of page table, flags are entry flags
        mov dword[edi], ebx     ; Write ebx to PT[x] = a.append(flags)
        add ebx, 0x1000         ; Increment address of ebx (a+1)
        add edi, 8              ; Increment page table location (8 bytes entries)
        loop .add_page_entry_protected

    mov edi, 0x5000             ; Go to PT[1] ; (2MB-4MB)
    mov ebx, 0x00200003         ; EBX has address 0x0000, flags: 0x0003
    mov ecx, 512                ; 512 times (512 entries in a table)

    .add_page_entry_protected2:
        ; a = address, x = index of page table, flags are entry flags
        mov dword[edi], ebx     ; Write ebx to PT[x] = a.append(flags)
        add ebx, 0x1000         ; Increment address of ebx (a+1)
        add edi, 8              ; Increment page table location (8 bytes entries)
        loop .add_page_entry_protected2
    
    mov edi, 0x6000             ; Go to PT[2] ; (4MB-6MB)
    mov ebx, 0x00400003         ; EBX has address 0x0000, flags: 0x0003
    mov ecx, 512                ; 512 times (512 entries in a table)

    .add_page_entry_protected3:
        ; a = address, x = index of page table, flags are entry flags
        mov dword[edi], ebx     ; Write ebx to PT[x] = a.append(flags)
        add ebx, 0x1000         ; Increment address of ebx (a+1)
        add edi, 8              ; Increment page table location (8 bytes entries)
        loop .add_page_entry_protected3

    ; Enable PAE paging
    mov eax, cr4
    or eax, 1 << 5 ; Set the PAE-bit, which is the 5th bit
    mov cr4, eax

    popad
    ret
