#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/io.h>

void cli(){
    __asm__ volatile ("cli" ::: "memory");
}

void sti(){
    if(interrupts_ready)
        __asm__ volatile ("sti" ::: "memory");
}

void qemu_shutdown(void) {
    outw(0x604, 0x2000);
}
