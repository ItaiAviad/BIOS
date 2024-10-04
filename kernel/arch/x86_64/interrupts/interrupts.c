#include <arch/x86_64/interrupts.h>

void cli(){
    __asm__ volatile ("cli" ::: "memory");
}

void sti(){
    __asm__ volatile ("sti" ::: "memory");
}