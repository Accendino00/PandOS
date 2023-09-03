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

static const char *exc_codes[] = {
    "INT",
    "MOD",
    "TLBL",
    "TLBS",
    "ADeS",
    "ADeL",
    "IBE",
    "DBE",
    "SYS",
    "BP",
    "RI",
    "CpU",
    "OV"};

static const char *sys_codes[] = {
    "CREATEPROCESS",
    "TERMPROCESS",
    "PASSEREN",
    "VERHOGEN",
    "DOIO",
    "GETTIME",
    "CLOCKWAIT",
    "GETSUPPORTPTR",
    "GETPROCESSID",
    "GETCHILDREN"};

void exceptionHandler()
{
    state_t *excState = (state_t *)BIOSDATAPAGE;

    int excCode = CAUSE_GET_EXCCODE(excState->cause);
    switch (excCode)
    {
    case EXC_INT:
    case EXC_SYS:
        break;
    default:
        PRINT_DEBUG("Exception %s: ", exc_codes[excCode]);
    }

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

void __terminateProcess(pcb_t *p)
{
    pcb_t *child;
    PRINT_DEBUG("Process %d has %d children\n", p->p_pid, list_size(&p->p_child));

    while ((child = removeChild(p)) != NULL)
    {
        outProcQ(getReadyQueue(), child);
        __terminateProcess(child);
    }

    bool isSoftBlocked = ((memaddr)p->p_semAdd >= (memaddr)getDevSem(0) &&  (memaddr)p->p_semAdd <= (memaddr)getDevSem(47)) || (memaddr)p->p_semAdd == (memaddr)getPseudoClockSem();

    if(isSoftBlocked) {
        decrementSoftBlockCount();
    }

    outBlocked(p);

    PRINT_DEBUG("Process %d terminated\n", p->p_pid);
    freePcb(p);
    decrementProcessCount();
}

void terminateProcess(pcb_t *p)
{
    outChild(p);
    __terminateProcess(p);

    schedule();
}

void syscallHandler(state_t *excState)
{
    excState->pc_epc += WORDLEN;
    if ((getSTATUS() & STATUS_KUp) >> STATUS_KUp_BIT == ON)
    {
        PRINT_DEBUG("Syscall called in user mode: ");
        excState->cause = (EXC_RI << CAUSE_EXCCODE_BIT);
        passUpOrDie(GENERALEXCEPT);
    }
    else
    {

        // PRINT_DEBUG("Syscall %s: \n", sys_codes[excState->reg_a0 - 1]);

        switch (excState->reg_a0)
        {
        case CREATEPROCESS:
        {
            pcb_t *new_p = allocPcb();
            if (new_p == NULL){
                excState->reg_v0 = -1;
                panic();
            }

            new_p->p_pid = getNewPid();
            enqueueReady(new_p);
            incrementProcessCount();

            insertChild(currentProc(), new_p);

            state_t *new_p_state = (state_t *)excState->reg_a1;
            support_t *new_p_support = (support_t *)excState->reg_a2;
            nsd_t *new_p_nsd = (nsd_t *)excState->reg_a3;

            new_p->p_s = *new_p_state;
            new_p->p_supportStruct = new_p_support;

            if (!addNamespace(new_p, new_p_nsd))
            {
                for (int i = 0; i < NS_TYPE_MAX; i++)
                {
                    addNamespace(new_p, getNamespace(currentProc(), i));
                }
            }

            excState->reg_v0 = new_p->p_pid;
            PRINT_DEBUG("Created process %d\n", new_p->p_pid);

            break;
        }
        case TERMPROCESS:
        {
            int pid = (int)excState->reg_a1;
            PRINT_DEBUG("Process %d has requested termination of %d\n", currentProc()->p_pid, pid == 0 ? currentProc()->p_pid : pid);

            pcb_t *toBeKilled = pid == 0 ? currentProc() : findPcb(pid, getReadyQueue());
            if(toBeKilled == NULL) {
                panic("Process %d has requested termination of %d, but it does not exist\n", currentProc()->p_pid, pid == 0 ? currentProc()->p_pid : pid);
            }

            terminateProcess(toBeKilled);
            break;
        }
        case PASSEREN:
        {
            int *sem = (int *)excState->reg_a1;

            if (*sem == 0)
            {
                insertBlocked(sem, currentProc());
                memcpy(&currentProc()->p_s, excState, sizeof(state_t));
                schedule();
            }
            else if (headBlocked(sem) != NULL)
            {
                pcb_t *proc = removeBlocked(sem);
                enqueueReady(proc);
            }
            else
            {
                (*sem) = 0;
            }

            break;
        }
        case VERHOGEN:
        {
            int *sem = (int *)excState->reg_a1;

            if (*sem == 1)
            {
                insertBlocked(sem, currentProc());
                memcpy(&currentProc()->p_s, excState, sizeof(state_t));
                schedule();
            }
            else if (headBlocked(sem) != NULL)
            {
                pcb_t *proc = removeBlocked(sem);
                enqueueReady(proc);
            }
            else
            {
                (*sem) = 1;
            }
            break;
        }
        case DOIO:
        {

            unsigned int *cmdAddr = (unsigned int *)excState->reg_a1;
            unsigned int *cmdValues = (unsigned int *)excState->reg_a2;

            currentProc()->test = cmdValues;

            int int_line = ((memaddr)cmdAddr - (memaddr)DEV_REG_START) / (DEV_REG_SIZE * DEVPERINT) + DEV_IL_START;
            int temp = (int_line == IL_TERMINAL ? DEVPERINT : DEVPERINT * 2);
            int dev_num = (int)((int)cmdAddr - (int)DEV_REG_ADDR(int_line, 0)) / temp;
            int dev_index = EXT_IL_INDEX(int_line) * DEVPERINT + dev_num;
            semaphore_t *sem;
            if (int_line == IL_TERMINAL)
            {
                termreg_t *base;
                if (dev_index % 2 != 0)
                {
                    base = (termreg_t *)((int)cmdAddr - (DEV_REG_SIZE) / 2);
                    base->transm_command = cmdValues[COMMAND];
                }
                else
                {
                    base = (termreg_t *)cmdAddr;
                    base->recv_command = cmdValues[COMMAND];
                }
            }
            sem = getDevSem(dev_index);
            P(sem);
            memcpy(&currentProc()->p_s, excState, sizeof(state_t));
            schedule();
            break;
        }
        case GETTIME:
            excState->reg_v0 = currentProc()->p_time;
            break;
        case CLOCKWAIT:
            P(getPseudoClockSem());
            memcpy(&currentProc()->p_s, excState, sizeof(state_t));
            schedule();
            break;
        case GETSUPPORTPTR:
            excState->reg_v0 = (int)currentProc()->p_supportStruct;
            break;
        case GETPROCESSID:
        {
            int parent = (int)excState->reg_a1;

            if (parent)
            {
                nsd_t *nsd = getNamespace(currentProc(), NS_PID);
                if (nsd == getNamespace(currentProc()->p_parent, NS_PID))
                {
                    excState->reg_v0 = currentProc()->p_parent->p_pid;
                    break;
                }
                else
                {
                    excState->reg_v0 = 0;
                    break;
                }
            }
            else
            {
                excState->reg_v0 = currentProc()->p_pid;
                break;
            }
            break;
        }
        case GETCHILDREN:
        {
            int *children = (int *)excState->reg_a1;
            int size = (int)excState->reg_a2;

            int retVal = 0;

            if(!emptyChild(currentProc())) {
                pcb_t * child = list_first_entry(&currentProc()->p_child, pcb_t, p_sib);
                nsd_t *nsd = getNamespace(currentProc(), NS_PID);

                list_for_each_entry(child, &currentProc()->p_child, p_sib) {
                    if(nsd == getNamespace(child, NS_PID)) {
                        if(retVal < size) {
                            children[retVal] = child->p_pid;
                        }
                        retVal++;
                    }
                }
            }

            excState->reg_v0 = retVal;

            break;
        }
        default:
            PRINT_DEBUG("Unknown syscall (%d): ", excState->reg_a0);
            passUpOrDie(GENERALEXCEPT);
            break;
        }

        LDST(excState);
    }
}

void passUpOrDie(int excCode)
{
    PANDOS_ASSERT(excCode == PGFAULTEXCEPT || excCode == GENERALEXCEPT, "Invalid exception code passed to passUpOrDie\n");

    pcb_t *proc = currentProc();

    printf("%s (pid %d) [%s]\n", proc->p_supportStruct == NULL ? "terminating" : "Passing up", proc->p_pid, excCode == 1 ? "general" : "pgfault");
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