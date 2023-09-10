// Phase 1 includes
#include "ash.h"
#include "pcb.h"
#include "ns.h"
// umps includes
#include <umps/types.h>
#include <umps/libumps.h>
// Phase 2 includes
#include "scheduler.h"
#include "exception.h"
#include "devices.h"
#include "processor.h"
// Library includes
#include "print.h"

// Extern functions
extern void uTLB_RefillHandler();
extern void test();

/**
 * @brief This function initializes the kernel.
 * 
 * @param pc The entry point of the kernel.
 */
static inline void initKernel(memaddr pc)
{
    /* Initialize phase1 data structures */
    initPcbs();
    initASH();
    initNamespaces();
    initDevSems();

    /* Initialize the passupvector */
    passupvector_t *puv = (passupvector_t *)PASSUPVECTOR;
    puv->tlb_refill_handler = (memaddr)uTLB_RefillHandler;
    puv->tlb_refill_stackPtr = (memaddr)KERNELSTACK;
    puv->exception_handler = (memaddr)exceptionHandler;
    puv->exception_stackPtr = (memaddr)KERNELSTACK;
    
    // Initialize the scheduler (variables and ready queue)
    initScheduler();
    
    // Initialize the interval timer
    resetIntervalTimer();

    // Create entry point
    pcb_t *proc = allocPcb();
    if (proc == NULL)
    {
        PRINT_DEBUG("Error allocating pcb for entry point\n");
        panic();
    }
    enqueueReady(proc);
    incrementProcessCount();
    proc->p_pid = getNewPid();

    proc->p_s.pc_epc = proc->p_s.reg_t9 = pc;   // We set the entry point
    RAMTOP(proc->p_s.reg_sp);                   // We set the stack pointer
    // We set the status to: interrupts on, timer on, all interrupt lines on
    proc->p_s.status = ( IEPON | IMON | TEBITON);

    PRINT_DEBUG("Kernel initialized!\n");
}

/* Kernel entry point*/
int main()
{
    initKernel((memaddr)test);  // Initialize the kernel
    schedule();                 // Call the scheduler
}
