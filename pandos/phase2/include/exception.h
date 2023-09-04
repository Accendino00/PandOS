#ifndef EXC_H
#define EXC_H

#include <umps/types.h>

/**
 * @brief Function called to handle exceptions
 * 
 * @note The address of this function has been stored
 * in the Pass Up Vector. It is going to be called
 * when an exception occurs to handle it.
 */
void exceptionHandler();

/**
 * @brief Function called to handle certain exception if there is
 * a support structure defined for the process or terminate the 
 * process if there isn't.
 * 
 * @param excCode The code of the exception
 * 
 * @note We call this for the TLB exceptions, Program Traps, SYSCALL
 * exceptions numbered 11 and above.
 * 
 * @note If the user has a support structure, we pass on the 
 * exception handling, otherwise we handle it as a SYS2:
 * we terminate the process and all its progeny and we
 * call the scheduler.
 */
extern void passUpOrDie(int excCode);


// Defined in syscall.c, see documentation in syscall.h
extern void syscallHandler(state_t* excState);

#endif