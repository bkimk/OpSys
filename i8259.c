/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"

#define secondaryOffset 8

/* Interrupt masks to determine which interrupts are enabled and disabled */
/* Initially all are off */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */


/* Initialize the 8259 PIC */
void i8259_init(void) {

    outb(0xff, MASTER_8259_PORT + 1); /* mask all of 8259A-1 */
    outb(0xff, SLAVE_8259_PORT + 1); /* mask all of 8259A-2 */

    outb(ICW1, MASTER_8259_PORT); /* ICW1: select 8259A-1 init */
    outb(ICW2_MASTER, MASTER_8259_PORT + 1); /* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27*/
    outb(ICW3_MASTER, MASTER_8259_PORT + 1); /* 8259A-1 (the primary) has a secondary on IR2 */
    outb(ICW4, MASTER_8259_PORT + 1); /* primary expects normal EOI */
    outb(ICW1, SLAVE_8259_PORT + 0);   /* ICW1: select 8259A-2 init */
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);    /*ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f */
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);    /*8259A-2 is a secondary on primary's IR2 */
    outb(ICW4, SLAVE_8259_PORT + 1);  /* secondary's support for AEOI in flat mode is to be investigated */
    
    /* wait for 8259A to initialize */
    outb(master_mask, MASTER_8259_PORT + 1);    /* restore primary IRQ mask */
    outb(slave_mask, SLAVE_8259_PORT + 1);      /* restore secondary IRQ mask */

    /* enable interrupts on slave */
    enable_irq(PIC_CASCADE_IR);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {

	if (irq_num & secondaryOffset){                               //Checking to see if IRQ num is from secondary PIC
        slave_mask &= ~(1 << (irq_num-secondaryOffset));          // find offset for secondary IRQ
		outb(slave_mask, SLAVE_8259_PORT + 1);
    }
	else {                                          //else IRQ num is from first PIC
        master_mask &= ~(1 << irq_num);             // find offset for primary IRQ
		outb(master_mask, MASTER_8259_PORT + 1);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {

	if (irq_num & secondaryOffset){                                 //Checking to see if IRQ num is from secondary PIC
        slave_mask |= (1 << (irq_num-secondaryOffset));             // find offset for secondary IRQ
		outb(slave_mask, SLAVE_8259_PORT + 1);
    }
	else {                                                          //else IRQ num is from first PIC
        master_mask |= (1 << irq_num);                              // find offset for primary IRQ
		outb(master_mask, MASTER_8259_PORT + 1);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
/* remove the interrupt from the in-service mask
 * check for raised interrupt lines that should be reporteed to processor
 */
void send_eoi(uint32_t irq_num) {

    if (irq_num & secondaryOffset) {
        slave_mask |= (1 << (irq_num-secondaryOffset));
        inb(SLAVE_8259_PORT + 1); /* DUMMY */
        outb(slave_mask, SLAVE_8259_PORT + 1);
        outb(EOI+(irq_num&7),SLAVE_8259_PORT); /* 'Specific EOI' to slave_PIC */
        outb(EOI+PIC_CASCADE_IR, MASTER_8259_PORT); /* 'Specific EOI' to primary-IRQ2 */
    }
    else {
        master_mask |= (1 << (irq_num));
        inb(MASTER_8259_PORT + 1); /* DUMMY */
        outb(master_mask, MASTER_8259_PORT + 1);
        outb(EOI+irq_num, MASTER_8259_PORT); /* 'Specific EOI to primary */
    }
}
