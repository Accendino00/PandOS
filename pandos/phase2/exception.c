#include <exception.h>
#include <pandos_const.h>
#include <pandos_types.h>

#define WORD_SIZE 4

#include <umps/libumps.h>
#include <umps/cp0.h>
#include <umps/types.h>

#include <scheduler.h>

#include <pcb.h>
#include <print.h>
#include <memory.h>
#include <syscall.h>
#include <processor.h>
#include "assert.h"

void exception_handler()
{
    PRINT_DEBUG("Exception handler\n");
    cpu_t exc_start_time;

    scheduler_control_t scheduler_control;

    if (get_current_process() != NULL)
    {
        store_tod(&exc_start_time);
        get_current_process()->p_time += (exc_start_time - get_start_time());
        memcpy(&(get_current_process()->p_s), (state_t *)BIOSDATAPAGE, sizeof(state_t));
    }

    switch (CAUSE_GET_EXCCODE(getCAUSE()))
    {
    case 0:
        PRINT_DEBUG("Interrupt with code %d\n", getCAUSE());
        // TO-DO: Interrupt handler
        break;
    case 1:
    case 2:
    case 3:
        scheduler_control = passup_or_terminate(PGFAULTEXCEPT);
        break;
    case 8:
    {
        cpu_t sys_start_time;
        cpu_t sys_finish_time;
        store_tod(&sys_start_time);
        scheduler_control = syscall_handler();
        store_tod(&sys_finish_time);
        get_current_process()->p_time += (sys_finish_time - sys_start_time);

        get_current_process()->p_s.pc_epc += WORD_SIZE;
        get_current_process()->p_s.reg_t9 += WORD_SIZE;
        break;
    }
    default:
        scheduler_control = passup_or_terminate(GENERALEXCEPT);
        break;
    }
    schedule(scheduler_control.pcb, scheduler_control.ret);
}

void init_passupvector(memaddr tlb_refill_handler, memaddr exception_handler)
{
    passupvector_t *const passupvector = (passupvector_t *)PASSUPVECTOR;
    passupvector->tlb_refill_handler = tlb_refill_handler;
    passupvector->tlb_refill_stackPtr = KERNELSTACK;
    passupvector->exception_handler = exception_handler;
    passupvector->exception_stackPtr = KERNELSTACK;
}

scheduler_control_t passup_or_terminate(int cause)
{
    PANDOS_ASSERT(cause == PGFAULTEXCEPT || cause == GENERALEXCEPT, "Invalid cause code! (%d)", cause);

    pcb_t *const current_process = get_current_process();

    if (current_process == NULL)
        return (scheduler_control_t){NULL, SCH_DO_NOTHING};
    else if (current_process->p_supportStruct == NULL)
    {
        PRINT_DEBUG("Process %d terminated\n", current_process->p_pid);
        kill_children(current_process);
    }
    else
    {
        PRINT_DEBUG("Passing up exception %d to support struct\n", cause);
        memcpy(&(current_process->p_supportStruct->sup_exceptState[cause]), (state_t *)BIOSDATAPAGE, sizeof(state_t));
        load_context(&(current_process->p_supportStruct->sup_exceptContext[cause]));
    }

    return (scheduler_control_t){NULL, SCH_DO_NOTHING};
}