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

/**
 * @brief Enqueues a process in the ready queue.
*/
extern void enqueueReady(pcb_t* proc);


extern unsigned int getNewPid();
extern void incrementProcessCount();
extern void decrementProcessCount();
extern void incrementSoftBlockCount();
extern void decrementSoftBlockCount();
extern cpu_t getStartTime();

extern  pcb_t* const currentProc();

// extern int __kill_process(pcb_t *p);
// extern int terminateProcess(pcb_t *p);

struct list_head *getReadyQueue();

#endif