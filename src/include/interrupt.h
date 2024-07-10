#ifndef __INTERRUPT_H
#define __INTERRUPT_H
#include "types.h"

typedef void* intr_handler;
void interrupt_init(void);
void register_handler(u8 num, intr_handler handler);

enum intr_status { INTR_OFF, INTR_ON };

enum intr_status intr_get_status(void);
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);

#endif