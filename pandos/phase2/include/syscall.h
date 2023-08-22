#ifndef SYSCALL_H
#define SYSCALL_H

#include <scheduler.h>

// macros for syscall parameters handling

#define RETURN_SYS(x, type) get_current_process()->p_s.reg_v0 = (type)(x)
#define SYSCALL_TYPE (get_current_process()->p_s.reg_a0)
#define FIRST_ARG(type) (type)(get_current_process()->p_s.reg_a1)
#define SECOND_ARG(type) (type)(get_current_process()->p_s.reg_a2)
#define THIRD_ARG(type) (type)(get_current_process()->p_s.reg_a3)

// syscalls codes

#define CREATEPROCESS 1
#define TERMPROCESS 2
#define PASSEREN 3
#define VERHOGEN 4
#define DOIO 5
#define GETTIME 6
#define CLOCKWAIT 7
#define GETSUPPORTPTR 8
#define GETPROCESSID 9
#define GETCHILDREN 10

/**
 * @brief Handles a syscall
 *
 * @return scheduler_control_t The action to be taken by the scheduler after the syscall
 *
 * @note The function returns a <code>{NULL, SCH_DO_NOTHING}</code>, if standard scheduler behaviour is to be applied
 */
scheduler_control_t syscall_handler();

// System call functions

/**
 * @brief Handles the CREATEPROCESS syscall
 *
 * This system call creates a new process as a child of the calling process.
 *
 * @param state_t* state The state of the new process
 * @param support_t* support The support struct of the new process
 * @param nsd_t* namespace The namespace of the new process
 *
 * @return scheduler_control_t The action to be taken by the scheduler after the syscall
 *
 * @note This syscall sets the new process as the active one.
 *
 */
scheduler_control_t sys_create_process(); // CREATEPROCESS

/**
 * @brief Handles the TERMPROCESS syscall
 *
 * This system call terminates the calling process and all of its children.
 *
 * @param int pid The pid of the process to terminate. If 0, the calling process is terminated.
 */
void sys_terminate_process(); // TERMPROCESS

void sys_passeren();                    // PASSEREN
void sys_verhogen();                    // VERHOGEN
void sys_doio();                        // DOIO

/**
 * @brief Handles the GETTIME syscall.
 * 
 * This system call returns the accumulated cpu time of the calling process.
 * 
 * @return cpu_t The accumulated time of the calling process.
*/
scheduler_control_t sys_get_cpu_time(); // GETTIME

void sys_wait_clock();                  // CLOCKWAIT

/**
 * @brief Handles the GETSUPPORTPTR syscall
 *
 * This system call returns the support struct of the calling process.
 *
 * @return support_t* The support struct of the calling process
 */
void sys_get_support_data(); // GETSUPPORTPTR

void sys_get_process_id(); // GETPROCESSID
void sys_get_children();   // GETCHILDREN

#endif