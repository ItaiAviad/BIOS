#include <arch/x86_64/interrupts.h>

void cli(){
    __asm__ volatile("cli");
}

void sti(){
    __asm__ volatile("sti");
}