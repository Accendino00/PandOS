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
    state_t *excState = (state_t *)BIOSDATAPAGE;
    unsigned int excCode = CAUSE_GET_EXCCODE(excState->cause);

    // update process time
    if (currentProc() != NULL)
    {
        cpu_t excTime;
        STCK(excTime);
        currentProc()->p_time += (excTime - getStartTime());
    }

    switch (excCode)
    {
    case EXC_INT:
        interruptHandler(excState);
        break;
    case EXC_MOD:
    case EXC_TLBL:
    case EXC_TLBS:
        passUpOrDie(PGFAULTEXCEPT);
        break;
    case EXC_SYS:
        syscallHandler(excState);
        break;
    default:
        passUpOrDie(GENERALEXCEPT);
        break;
    }
}



void passUpOrDie(int excCode)
{
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