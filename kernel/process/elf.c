#include <elf.h>
// libc
#include <memory.h>
#include <random.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// drivers
#include <pci.h>
#include <disk.h>

void* readelf(void* dst, bool print) { 
    Elf64_Ehdr *elf_ehdr = (Elf64_Ehdr*) readelf_header(print);
    if (!elf_ehdr) {
        free(elf_ehdr);
        return NULL;
    }
    
    void *elf_ps = readelf_ps(elf_ehdr);
    if (!elf_ps) {
        free(elf_ehdr);
        free(elf_ps);
        return NULL;
    }
    Elf64_Phdr *elf_phdr = (Elf64_Phdr*) elf_ps;
    memcpy(dst, elf_ps + elf_ehdr->e_phentsize, elf_phdr->p_filesz); // copy program segment

    free(elf_ehdr);
    free(elf_ps);

    return dst;
}

void* readelf_header(bool print) {
    Elf64_Ehdr* elf_ehdr = malloc(sizeof(Elf64_Ehdr));
    if (!elf_ehdr)
        return NULL;

    read(0, 0, sizeof(Elf64_Ehdr), elf_ehdr);

    /* Verify ELF Header  */

    /* EI */
    // Magic
    for (int ei = 0; ei < SELFMAG; ++ei)
        if (elf_ehdr->e_ident[ei] != ELFMAG[ei])
            return NULL;

    // Class (verify 64bit)
    if (elf_ehdr->e_ident[EI_CLASS] != ELFCLASS64)
        return NULL;

    // Data (verify LSB)
    if (elf_ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
        return NULL;

    if (!print)
        return (void*) elf_ehdr;

    // print elf header (`readelf -h`)
    printf("ELF Header: \n");
    printf("  Magic:    "); for (int ei = 0; ei < EI_NIDENT; ++ei) printf("%x ", elf_ehdr->e_ident[ei]); printf("\n");
    printf("  Class: %s\n", elf_ehdr->e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32");
    printf("  Data: %s\n", elf_ehdr->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian");
    printf("  Version: %d\n", elf_ehdr->e_ident[EI_VERSION]);
    printf("  OS/ABI: %s\n", elf_ehdr->e_ident[EI_OSABI] == ELFOSABI_NONE ? "UNIX - System V" : "UNIX - System V");
    printf("  ABI Version: %d\n", elf_ehdr->e_ident[EI_ABIVERSION]);
    printf("  Type: %s\n", elf_ehdr->e_type == ET_EXEC ? "EXEC (Executable file)" : "DYN (Shared object file)");
    printf("  Machine: %s\n", elf_ehdr->e_machine == EM_X86_64 ? "Advanced Micro Devices x86-64" : "Unknown");
    printf("  Version: %p\n", elf_ehdr->e_version);
    printf("  Entry point address: %p\n", elf_ehdr->e_entry);
    printf("  Start of program headers: %d (bytes into file)\n", elf_ehdr->e_phoff);
    printf("  Start of section headers: %d (bytes into file)\n", elf_ehdr->e_shoff);
    printf("  Flags: %p\n", elf_ehdr->e_flags);
    printf("  Size of this header: %d (bytes)\n", elf_ehdr->e_ehsize);
    printf("  Size of program headers: %d (bytes)\n", elf_ehdr->e_phentsize);
    printf("  Number of program headers: %d\n", elf_ehdr->e_phnum);
    printf("  Size of section headers: %d (bytes)\n", elf_ehdr->e_shentsize);
    printf("  Number of section headers: %d\n", elf_ehdr->e_shnum);
    printf("  Section header string table index: %d\n", elf_ehdr->e_shstrndx);

    return (void*) elf_ehdr;
}

void* readelf_ps(Elf64_Ehdr *elf_ehdr) {
    Elf64_Phdr* elf_phdr = malloc(elf_ehdr->e_phentsize);
    read(0, elf_ehdr->e_phoff, elf_ehdr->e_phentsize, elf_phdr);
    
    // verify program header
    if (elf_phdr->p_type != PT_LOAD)
        return NULL;

    // program header, program segment
    void* elf_ps = malloc(elf_ehdr->e_phentsize + elf_phdr->p_filesz);
    memcpy(elf_ps, elf_phdr, elf_ehdr->e_phentsize); // copy program header
    read(0, elf_phdr->p_offset, elf_phdr->p_filesz, (void*) ((uint64_t) elf_ps + (uint64_t) elf_ehdr->e_phentsize)); // copy program segment

    free(elf_phdr);

    return elf_ps;
}
