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
#include "interrupt.h"

#include <umps/arch.h>

void exception_handler()
{
    int status = getSTATUS();
    status &= ~IECON; // Disable interrupts
    setSTATUS(status);

    PRINT_DEBUG("Exception handler --> cause: %d\n", CAUSE_GET_EXCCODE(getCAUSE()));
    cpu_t exc_start_time;
    scheduler_control_t scheduler_control;

    if (get_current_process() != NULL)
    {
        store_tod(&exc_start_time);
        cpu_t new_acc_time = exc_start_time - get_start_time();
        get_current_process()->p_time += new_acc_time;
        memcpy(&(get_current_process()->p_s), (state_t *)BIOSDATAPAGE, sizeof(state_t));
    }

    switch (CAUSE_GET_EXCCODE(getCAUSE()))
    {
    case EXC_INT:
        // Case {0}   : Interrupts
        // We pass it to the interrupt handler
        // TO-DO: Interrupt handler
        scheduler_control = interrupt_handler(getCAUSE());
        break;
    case EXC_MOD:
    case EXC_TLBL:
    case EXC_TLBS:
        // Case {1-3} : TLB Exceptions
        // We pass it to the pass up or die function (Where we have the TLB exception handler)
        scheduler_control = passup_or_terminate(PGFAULTEXCEPT);
        break;
    case EXC_SYS:
        // Case {8}   : SYSCALL
        // We pass it to the syscall handler
        {
            // We add the time to the process because the time it takes
            // to execute a syscall is time "spent" by the process, since
            // its the one requesting it, whereas other exceptions are not.
            cpu_t sys_start_time;
            cpu_t sys_finish_time;
            store_tod(&sys_start_time);
            scheduler_control = syscall_handler();
            store_tod(&sys_finish_time);
            get_current_process()->p_time += (sys_finish_time - sys_start_time);

            get_current_process()->p_s.pc_epc += WORD_SIZE;

            // #TODO - da rimuovere se funziona senza, da aggiungere se non funziona {slide 40}
            // get_current_process()->p_s.reg_t9 += WORD_SIZE;
            break;
        }
    default:
        // Case {4-7, 9-12} : Program Traps
        // We pass it up to the pass up or die function (Where we have the program traps exception handler)
        scheduler_control = passup_or_terminate(GENERALEXCEPT);
        break;
    }

    status = getSTATUS();
    status |= IEPBITON; // Enable interrupts
    setSTATUS(status);

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
        return (scheduler_control_t){NULL, SCH_BLOCK};
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

    return (scheduler_control_t){NULL, SCH_BLOCK};
}