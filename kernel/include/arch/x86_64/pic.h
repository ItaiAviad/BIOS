// PIC - Programmable Interrupt Controller

#include <stdio.h>
#include <stdlib.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/scs1.h>
#include <arch/x86_64/interrupts.h>
#include <arch/x86_64/pit.h>

#ifndef PIC_H
#define PIC_H

#define PIC1		0x20		// IO base address for master PIC
#define PIC2		0xA0		// IO base address for slave PIC
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC1_OFFSET	0x20
#define PIC2_OFFSET	0x28

#define PIC_EOI		0x20		// End-of-interrupt command code

// Reinitialize the PIC controllers, giving them specified vector offsets
// rather than 8h and 70h, as configured by default
#define ICW1_ICW4	0x01		// Indicates that ICW4 will be present
#define ICW1_SINGLE	0x02		// Single (cascade) mode
#define ICW1_INTERVAL4	0x04    // Call address interval 4 (8)
#define ICW1_LEVEL	0x08		// Level triggered (edge) mode
#define ICW1_INIT	0x10		// Initialization - required!

#define ICW4_8086	0x01		// 8086/88 (MCS-80/85) mode
#define ICW4_AUTO	0x02		// Auto (normal) EOI
#define ICW4_BUF_SLAVE	0x08	// Buffered mode/slave
#define ICW4_BUF_MASTER	0x0C	// Buffered mode/master
#define ICW4_SFNM	0x10		// Special fully nested (not)

#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */

#define IRQ_PIT 0x0
#define IRQ_KEYBOARD 0x1

void pic_send_eoi(uint8_t irq);
/**
 * @brief 
 * 
 * @param offset1 vector offset for master PIC vectors on the master become offset1..offset1+7
 * @param offset2 same for slave PIC: offset2..offset2+7
 */
void pic_init(int offset1, int offset2);
void pic_disable(void);
void irq_set_mask(uint8_t IRQline);
void irq_clear_mask(uint8_t IRQline);

uint16_t __pic_get_irq_reg(int ocw3);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif
