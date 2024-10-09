[bits 32]

init_paging:
    pushad
    ; Clear the memory area using rep stosd
    mov edi, 0x1000
    xor eax, eax
    mov ecx, 4096
    rep stosd

    ; Setup PML4T
    mov edi, 0x1000
    mov dword[edi], 0x2003      ; PML4T[0] points to PDPT at 0x2000, flags: present + writable

    ; Setup PDPT for 1GB pages
    mov edi, 0x2000
    mov dword[edi], 0x00000083  ; PDPT[0] points to 0x0000000000 (1GB page), flags: present + writable + large page
    mov dword[edi+8], 0x40000083 ; PDPT[1] points to 0x4000000000 (1GB page), flags: present + writable + large page
    mov dword[edi+16], 0x80000083 ; PDPT[2] points to 0x8000000000 (1GB page), flags: present + writable + large page

    ; Enable PAE paging
    mov eax, cr4
    or eax, 1 << 5              ; Set the PAE-bit, which is the 5th bit
    or eax, 1 << 4              ; Enable PSE (Page Size Extension)
    mov cr4, eax

    ; Load PML4T address into CR3
    mov eax, 0x1000
    mov cr3, eax
    
    popad
    ret
