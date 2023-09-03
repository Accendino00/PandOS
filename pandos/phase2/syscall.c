#include "syscall.h"
#include "scheduler.h"
#include "print.h"
#include "pandos_const.h"
#include "pandos_types.h"
#include <umps/cp0.h>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <umps/types.h>
#include "exception.h"
#include "devices.h"
#include "pcb.h"
#include "ash.h"
#include "ns.h"
#include "memory.h"
#include "interrupt.h"

// macros for syscall parameters

#define FIRST_PARAM(type) (type) excState->reg_a1
#define SECOND_PARAM(type) (type) excState->reg_a2
#define THIRD_PARAM(type) (type) excState->reg_a3

#define RETURN_SYS_VALUE(x, type) excState->reg_v0 = (type)x

/**
 * @brief Handles the CREATEPROCESS syscall.
 *
 * @param state The state of the new process.
 * @param supportStruct The support struct of the new process.
 * @param nsd The namespace descriptor of the new process.
 *
 * @return The PID of the new process if succesful, -1 otherwise.
 */
static int createProcess(state_t *state, support_t *supportStruct, nsd_t *nsd)
{
    pcb_t *new_p = allocPcb();
    if (new_p == NULL)
    {
        return -1;
    }

    new_p->p_pid = getNewPid();
    enqueueReady(new_p);
    incrementProcessCount();

    insertChild(currentProc(), new_p);

    new_p->p_s = *state;
    new_p->p_supportStruct = supportStruct;

    if (!addNamespace(new_p, nsd))
    {
        for (int i = 0; i < NS_TYPE_MAX; i++)
        {
            addNamespace(new_p, getNamespace(currentProc(), i));
        }
    }

    return new_p->p_pid;
}

/**
 * @brief Helper function for terminateProcess.
 */
static void __terminateProcess(pcb_t *p)
{
    pcb_t *child;

    while ((child = removeChild(p)) != NULL)
    {
        outProcQ(getReadyQueue(), child);
        __terminateProcess(child);
    }

    bool isSoftBlocked = ((memaddr)p->p_semAdd >= (memaddr)getDevSem(0) && (memaddr)p->p_semAdd <= (memaddr)getDevSem(47)) || (memaddr)p->p_semAdd == (memaddr)getPseudoClockSem();

    if (isSoftBlocked)
    {
        decrementSoftBlockCount();
    }

    outBlocked(p);

    freePcb(p);
    decrementProcessCount();
}

void terminateProcess(pcb_t *p)
{
    outChild(p);
    __terminateProcess(p);

    schedule();
}

/**
 * @brief Handles the PASSEREN syscall.
 *
 * @param sem The semaphore on which to perform the operation.
 *
 * @return 1 if the operation was blocking, 0 otherwise.
 */
static int passeren(semaphore_t *sem)
{
    if (*sem == 0)
    {
        insertBlocked(sem, currentProc());
        return 1;
    }
    else if (headBlocked(sem) != NULL)
    {
        pcb_t *p = removeBlocked(sem);
        enqueueReady(p);
    }
    else
    {
        (*sem)--;
    }

    return 0;
}

/**
 * @brief Handles the VERHOGEN syscall.
 *
 * @param sem The semaphore on which to perform the operation.
 *
 * @return 1 if the operation was blocking, 0 otherwise.
 */
static int verhogen(semaphore_t *sem)
{
    if (*sem == 1)
    {
        insertBlocked(sem, currentProc());
        return 1;
    }
    else if (headBlocked(sem) != NULL)
    {
        pcb_t *proc = removeBlocked(sem);
        enqueueReady(proc);
    }
    else
    {
        (*sem)++;
    }
    return 0;
}

/**
 * @brief Handles the DOIO syscall.
 *
 * @param cmdAddr The address of the device register.
 * @param cmdValues The values to write to the device register.
 *
 * @note The status register of the device is copied back into cmdValues after the operation.
 * @note The return value of the operation is handled by the interrupt handler.
 */
static void doIO(unsigned int *cmdAddr, unsigned int *cmdValues)
{
    currentProc()->p_savedDeviceStatus = cmdValues;

    int int_line = ((memaddr)cmdAddr - (memaddr)DEV_REG_START) / (DEV_REG_SIZE * DEVPERINT) + DEV_IL_START;
    int temp = (int_line == IL_TERMINAL ? DEVPERINT : DEVPERINT * 2);
    int dev_num = (int)((int)cmdAddr - (int)DEV_REG_ADDR(int_line, 0)) / temp;
    int dev_index = EXT_IL_INDEX(int_line) * DEVPERINT + dev_num;

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
    } else {
        dtpreg_t *base = (dtpreg_t *)cmdAddr;
        base->command = cmdValues[COMMAND];
        base->data0 = cmdValues[DATA0];
        base->data1 = cmdValues[DATA1];
    }
    semaphore_t *sem = getDevSem(dev_index);
    P(sem);
}

/**
 * @brief Handles the GETPROCESSID syscall.
 * 
 * @param parent If 1, returns the PID of the parent process if it is in the same namespace, 0 otherwise. In any other case it returns the calling process PID.
*/
static int getProcessID(int parent)
{
    if (parent)
    {
        nsd_t *nsd = getNamespace(currentProc(), NS_PID);
        if (nsd == getNamespace(currentProc()->p_parent, NS_PID))
            return currentProc()->p_parent->p_pid;
        else
            return 0;
    }
    else
        return currentProc()->p_pid;
}

/**
 * @brief Handles the GETCHILDREN syscall.
 * 
 * @param children The array in which to store the children PIDs.
 * @param size The maximum number of children PIDs to store.
 * 
 * @return The number of children PIDs stored in the array.
 * 
 * @note Only the children in the same namespace as the calling process are considered.
 * @note The returned value could be greater than size. This means that the array was not big enough to store all the children PIDs.
*/
static int getChildren(int* children, int size) {
    int retVal = 0;

    if (!emptyChild(currentProc()))
    {
        pcb_t *child = list_first_entry(&currentProc()->p_child, pcb_t, p_sib);
        nsd_t *nsd = getNamespace(currentProc(), NS_PID);

        list_for_each_entry(child, &currentProc()->p_child, p_sib)
        {
            if (nsd == getNamespace(child, NS_PID))
            {
                if (retVal < size)
                {
                    children[retVal] = child->p_pid;
                }
                retVal++;
            }
        }
    }
    return retVal;
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
        switch (excState->reg_a0)
        {
        case CREATEPROCESS:
        {
            state_t *state = FIRST_PARAM(state_t *);
            support_t *supportStruct = SECOND_PARAM(support_t *);
            nsd_t *nsd = THIRD_PARAM(nsd_t *);

            int pid = createProcess(state, supportStruct, nsd);
            RETURN_SYS_VALUE(pid, int);
            break;
        }
        case TERMPROCESS:
        {
            int pid = FIRST_PARAM(int);
            terminateProcess(pid == 0 ? currentProc() : findPcb(pid, getReadyQueue()));
            break;
        }
        case PASSEREN:
        {
            if (passeren(FIRST_PARAM(semaphore_t *)))
            {
                memcpy(&currentProc()->p_s, excState, sizeof(state_t));
                schedule();
            }
            break;
        }
        case VERHOGEN:
        {
            if (verhogen(FIRST_PARAM(semaphore_t *)))
            {
                memcpy(&currentProc()->p_s, excState, sizeof(state_t));
                schedule();
            }
            break;
        }
        case DOIO:
        {
            unsigned int *cmdAddr = FIRST_PARAM(unsigned int *);
            unsigned int *cmdValues = SECOND_PARAM(unsigned int *);

            doIO(cmdAddr, cmdValues);
            memcpy(&currentProc()->p_s, excState, sizeof(state_t));
            schedule();
            break;
        }
        case GETTIME:
            RETURN_SYS_VALUE(currentProc()->p_time, unsigned int);
            break;
        case CLOCKWAIT:
            P(getPseudoClockSem());
            memcpy(&currentProc()->p_s, excState, sizeof(state_t));
            schedule();
            break;
        case GETSUPPORTPTR:
            RETURN_SYS_VALUE(currentProc()->p_supportStruct, support_t *);
            break;
        case GETPROCESSID:
        {
            int pid = getProcessID(FIRST_PARAM(int));
            RETURN_SYS_VALUE(pid, int);
            break;
        }
        case GETCHILDREN:
        {
            int *children = FIRST_PARAM(int *);
            int size = SECOND_PARAM(int);

            int retVal = getChildren(children, size);
            RETURN_SYS_VALUE(retVal, int);
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