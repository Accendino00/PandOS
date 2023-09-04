#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <umps/types.h>
#include "devices.h"
#include "pcb.h"

#define RECV 0
#define TRANSMIT 1

/**
 * @brief P's the semaphore.
 * 
 * @param semaphore_t* The semaphore to P
 * 
 * @note If the semaphore is positive, it decrements it. 
 * If it is negative, it blocks the process.
 */
extern void P(semaphore_t *);


/**
 * @brief V's the semaphore and returns the unblocked process.
 * 
 * @param semaphore_t* The semaphore to V
 * 
 * @note It unblocks the first process in the semaphore's queue 
 * and enqueues the current one. If there are none, it sets the 
 * semaphore to 1.
 * 
 * @return pcb_t* The process that was unblocked. 
 */
extern pcb_t *V(semaphore_t *);

/**
 * @brief Interrupt handler
 * 
 * @param excState The processor state of the process that caused the exception
 * 
 * @note This function gets called from the exception handler.
 */
void interruptHandler(state_t *);

#endif