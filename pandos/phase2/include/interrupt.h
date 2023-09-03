#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <umps/types.h>
#include "devices.h"
#include "pcb.h"

#define RECV 0
#define TRANSMIT 1

extern void P(semaphore_t *);
extern pcb_t *V(semaphore_t *);

void interruptHandler(state_t *);

#endif