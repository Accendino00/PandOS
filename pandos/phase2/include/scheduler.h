#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"

/**
 * @brief Initializes the scheduler and its private variables.
 */
void initScheduler();

/**
 * @brief Schedules the next ready process.
 */
void schedule();



// The following function are defined as inline in scheduler.c
// (This is done for performance reasons)

/**
 * @brief Enqueues a process in the ready queue.
 */
extern void enqueueReady(pcb_t* proc);

/**
 * @brief Return the next available PID.
 * This PID has not been assigned to any process yet.
 * 
 * @return unsigned int New PID to be assigned to a process.
 */
extern unsigned int getNewPid();


extern void incrementProcessCount();
extern void decrementProcessCount();
extern void incrementSoftBlockCount();
extern void decrementSoftBlockCount();

/**
 * @brief Returns the TOD when the current process started.
 * 
 * @return cpu_t The TOD when the current process started.
 */
extern cpu_t getStartTime();

/**
 * @brief Returns a pointer to the current process executing.
 * 
 * @return pcb_t* const 
 */
extern  pcb_t* const currentProc();

struct list_head *getReadyQueue();

#endif