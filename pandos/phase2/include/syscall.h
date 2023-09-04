#ifndef SYSCALL_H
#define SYSCALL_H

#include <umps/types.h>
#include <pandos_types.h>

/**
 * @brief Handles a syscall.
 * 
 * @param excState The state of the processor at the time of the syscall.
 * 
 * @note Syscalls 1-10 are handled by this function, the others are handled by passUpOrDie.
 * @note If the syscall is called by a process in user mode, we also handle it with passUpOrDie.
*/
extern void syscallHandler(state_t* excState);

/**
 * @brief Handles the TERMPROCESS syscall.
 * 
 * @param p The process to be terminated.
 * 
 * @note This function is also used by the passUpOrDie function in the "die" 
 * part, which needs to terminate the process and all its progeny.
*/
extern void terminateProcess(pcb_t* p);

#endif