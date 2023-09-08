#include "exception.h"

#include <umps/types.h>
#include <umps/cp0.h>
#include <umps/libumps.h>
#include <umps/arch.h>
#include <umps/const.h>
#include "pandos_const.h"
#include "scheduler.h"
#include "memory.h"
#include "print.h"
#include "ash.h"
#include "ns.h"
#include "interrupt.h"
#include "devices.h"
#include "assert.h"
#include "util.h"
#include "syscall.h"

void exceptionHandler()
{
    // We get the processor state of the current process that caused the exception
    // and the exception code
    state_t *excState = (state_t *)BIOSDATAPAGE;
    unsigned int excCode = CAUSE_GET_EXCCODE(excState->cause);

    // Update of process time
    if (currentProc() != NULL)
    {
        cpu_t excTime;
        STCK(excTime);
        currentProc()->p_time += (excTime - getStartTime());
    }

    // We choose the handler based on the exception code
    switch (excCode)
    {
    case EXC_INT:   // Code 0 - Interrupt
        interruptHandler(excState);
        break;
    case EXC_MOD:   // Code 1 - TLB exception (write to read only)
    case EXC_TLBL:  // Code 2 - TLB exception (load)
    case EXC_TLBS:  // Code 3 - TLB exception (store)
        passUpOrDie(PGFAULTEXCEPT);
        break;
    case EXC_SYS:   // Code 8 - Syscall
        syscallHandler(excState);
        break;
    default:        // 4-7, 9-12 - Program Traps
        passUpOrDie(GENERALEXCEPT);
        break;
    }

    // Just in case. However, this should never be reached.
    LDST(excState);
}



void passUpOrDie(int excCode)
{
    // We make sure the exception code is valid
    PANDOS_ASSERT(excCode == PGFAULTEXCEPT || excCode == GENERALEXCEPT, "Invalid exception code passed to passUpOrDie\n");

    pcb_t *proc = currentProc();

    if (proc->p_supportStruct == NULL)
    {
        terminateProcess(proc);
        schedule();
    }
    else
    {
        // pass up
        memcpy(&(proc->p_supportStruct->sup_exceptState[excCode]), (state_t *)BIOSDATAPAGE, sizeof(state_t));
        LDCXT(proc->p_supportStruct->sup_exceptContext[excCode].stackPtr,
              proc->p_supportStruct->sup_exceptContext[excCode].status,
              proc->p_supportStruct->sup_exceptContext[excCode].pc);
    }
}