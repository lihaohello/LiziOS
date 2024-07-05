#ifndef __INTERRUPT_H
#define __INTERRUPT_H

typedef void *interrupt_handler;
void interrupt_init();

enum intr_status
{
    INTR_OFF,
    INTR_ON
};
enum intr_status intr_enable();
enum intr_status intr_disable();
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_get_status();

#endif
