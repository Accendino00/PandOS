#ifndef SCH_H
#define SCH_H

#include <pandos_const.h>
#include <pandos_types.h>

#define QPAGE 1024

#define IEPBITON 0x4
#define KUPBITON 0x8
#define KUPBITOFF 0xFFFFFFF7
#define TEBITON 0x08000000
#define CAUSEINTMASK 0xFD00

// ### SCHEDULER CONTROL CODES

// the returned process becomes the active one
#define SCH_PRESERVE 0

// the returned process is enqueued
#define SCH_ENQUEUE 1

// block the current process
#define SCH_BLOCK 0

/**
 * Struct for scheduler control; returned by exceptions (syscalls, interrupts and generic exceptions)
 */
typedef struct scheduler_control
{
    pcb_t *pcb; // the returned process; can be NULL
    int ret;    // the action to be made. See above for the possible values
} scheduler_control_t;

/**
 * @brief Initializes the scheduler.
 */
void init_scheduler();

/**
 * Initializes a process in kernel mode, with pc at the given address.
 *
 * @param pc The address of the process(function)
 */
void init_process(memaddr pc);

/**
 * @brief Enqueues a process into the ready queue.
 *
 * @param process The process to be enqueued.
 */
void enqueue_ready(pcb_t *process);

/**
 * @brief Dequeues a process from the ready queue.
 *
 * @return pcb_t* The dequeued process.
 */
pcb_t *dequeue_ready(pcb_t *p);

/**
 * @brief Creates a process in kernel mode.
 *
 * @return pcb_t* The created process.
 */
pcb_t *create_process();

/**
 * @brief Kills a single process.
 *
 * @param p The process to be killed.
 * @return An integer indicating if there were any errors, 0 if none.
 */
int kill_process(pcb_t *p);

/**
 * @brief Kills a process, with all of its progeny.
 *
 * @param p The process which family is to be killed.
 * @return An integrer indicating if there were any errors, 0 if none.
 */
int kill_children(pcb_t *p);

/**
 * @brief Schedules the process that are in the ready queue.
 *
 * @param new A new process to be taken into account. Can be NULL.
 * @param action What to do with the new process. See above for the codes.
 *
 * @note The parameters are actually a <code>scheduler_control_t</code>.
 */
void schedule(pcb_t *new, int action);

/**
 * @brief Returns the current active process.
 *
 * @return The active process.
 */
pcb_t *get_current_process();

/**
 * @brief Decrements the global process count by one.
 */
void decrement_process_count();

/**
 * @brief Increments the global process count by one.
 */
void increment_process_count();

void increment_softblocked_count();
void decrement_softblocked_count();

extern cpu_t get_start_time();

#endif