#ifndef EXC_H
#define EXC_H

#include <pandos_types.h>
#include <scheduler.h>

/**
 * @brief Exception handler
 * 
 * Handles either interrupts, syscalls and generic program traps 
*/
void exception_handler();

/**
 * @brief Initializes the passup vector
 * 
 * @param tlb_refill_handler The address of the TLB refill handler
 * @param exception_handler The address of the exception handler
*/
void init_passupvector(memaddr tlb_refill_handler, memaddr exception_handler);

/**
 * @brief Handles generic program traps and not yet implemented syscalls\n
 * 
 * If a process calls a syscall with code > 10 or an exception with code 4-7,9-12, the control should
 * be passed to the indicated support struct if it exists, otherwise the process should be terminated.
 * 
 * @param cause The cause of the exception
 * 
 * @return The control struct for the scheduler
*/
scheduler_control_t passup_or_terminate(int cause);

#endif