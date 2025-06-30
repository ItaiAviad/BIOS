#ifndef INTERRUPTS_H

#define INTERRUPTS_H

extern int interrupts_ready;

/**
* @brief Disable interrupts via the cli instruction
* @param none
*/

void cli();

/**
* @brief Enable interrupts via the sti instruction
* @param none
*/
void sti();

void qemu_shutdown(void);

#endif
