#ifndef _INTERRUPT_H
#define _INTERRUPT_H

void init_interrupts();
void register_interrupt_handler(int num, void (*handler)());

#endif
