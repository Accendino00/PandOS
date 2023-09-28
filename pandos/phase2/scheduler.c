#include "scheduler.h"
#include "pandos_const.h"
#include "pandos_types.h"

#include <umps/libumps.h>

#include "pcb.h"
#include "processor.h"
#include "print.h"
#include "util.h"
#include "ash.h"

HIDDEN   unsigned int process_count = 0;
HIDDEN   unsigned int soft_block_count = 0;
HIDDEN   pcb_t *current_active_process = NULL;
HIDDEN   struct list_head ready_queue;
HIDDEN   unsigned int pid_counter = 0;
HIDDEN   cpu_t start_time = 0;

struct list_head* getReadyQueue() {
    return &ready_queue;
}

void initScheduler() {
    process_count = 0;
    soft_block_count = 0;
    current_active_process = NULL;
    pid_counter = 1;
    STCK(start_time);
    mkEmptyProcQ(&ready_queue);
}

void schedule() {
    // Reset the current active process to none
    // We have inserted it in the ready queue before calling this function
    current_active_process = NULL;


    // If the ready queue is empty, we have to check different conditions
    if(emptyProcQ(&ready_queue)) {
        // If there are no processes, we halt the system
        if(process_count == 0) {
            HALT();
        }
        // If there are processes but they are all blocked, we wait for an interrupt
        else if (process_count > 0 && soft_block_count > 0){
            // We enable interrupts 
            // (and we disable the timer since the next interrupt musn't be a timer interrupt)
            setSTATUS(( IECON | IMON) & (~TEBITON));
            WAIT();
        }
        // If this is true, we have a deadlock
        else if(process_count > 0 && soft_block_count == 0){
            printf("Blocked processes: \n");
            getBlockedProcesses();

            panic("Deadlock detected! Scheduler: \n\tProcess count: %d\n\tBlocked processes: %d\n\tProcesses in ready queue: %d\n", process_count, soft_block_count, list_size(&ready_queue));
        }            
    }

    // We set the new process as the current active process,
    // reset the process local timer and load the state of the new process
    current_active_process = removeProcQ(&ready_queue);
    resetPLT();
    STCK(start_time);
    LDST(&(current_active_process->p_s));
}


inline unsigned int getNewPid() {
    return pid_counter++;
}

inline void enqueueReady(pcb_t* proc) {
    insertProcQ(&ready_queue, proc);
    if(proc == current_active_process)
        current_active_process = NULL;
}

inline void incrementProcessCount() {
    process_count++;
}

inline void decrementProcessCount() {
    process_count--;
}

inline void incrementSoftBlockCount() {
    soft_block_count++;
}

inline void decrementSoftBlockCount() {
    soft_block_count--;
}

inline pcb_t* const currentProc() {
    return current_active_process;
}

inline cpu_t getStartTime() {
    return start_time;
}