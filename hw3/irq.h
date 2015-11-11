#ifndef __IRQ_H__
#define __IRQ_H__

typedef void (*handler_t)(int);

void setup_irq_handler(int irq, handler_t handler);
void irq_mask(int irq);
void irq_unmask(int irq);
void irq_setup(void);

#endif /*__IRQ_H__*/
