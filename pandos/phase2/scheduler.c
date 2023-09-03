#include "scheduler.h"
#include "pandos_const.h"
#include "pandos_types.h"

#include <umps/libumps.h>

#include "pcb.h"
#include "processor.h"
#include "print.h"
#include "util.h"
#include "ash.h"

HIDDEN volatile unsigned int process_count = 0;
HIDDEN volatile unsigned int soft_block_count = 0;
HIDDEN volatile pcb_t *current_active_process = NULL;
HIDDEN volatile struct list_head ready_queue;
HIDDEN volatile unsigned int pid_counter = 0;
HIDDEN volatile cpu_t start_time = 0;

struct list_head* getReadyQueue() {
    return &ready_queue;
}

void initScheduler() {
    process_count = 0;
    soft_block_count = 0;
    current_active_process = NULL;
    pid_counter = 1;
    STCK(start_time);
    resetIntervalTimer();
    resetPLT();
    mkEmptyProcQ(&ready_queue);
}

void schedule() {
    current_active_process = NULL;
    if(emptyProcQ(&ready_queue)) {
        if(process_count == 0)
            HALT();
        else if (process_count > 0 && soft_block_count > 0){
            // interrupts on
            setSTATUS(( IECON | IMON) & (~TEBITON));
            WAIT();
        }
        else if(process_count > 0 && soft_block_count == 0){
            printf("Blocked processes: \n");
            getBlockedProcesses();

            panic("Deadlock detected! Scheduler: \n\tProcess count: %d\n\tBlocked processes: %d\n\tProcesses in ready queue: %d\n", process_count, soft_block_count, list_size(&ready_queue));
        }
            
    }

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
