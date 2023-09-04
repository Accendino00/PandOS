#include "interrupt.h"

#include <umps/libumps.h>
#include <umps/cp0.h>
#include <umps/arch.h>
#include <umps/const.h>

#include "scheduler.h"
#include "processor.h"
#include "ash.h"
#include "devices.h"
#include "memory.h"
#include "print.h"

inline void P(semaphore_t *s)
{
    if (*s <= 0)
    {
        insertBlocked(s, currentProc());
        incrementSoftBlockCount();
    }
    else
    {
        *s = 0;
    }
}

inline pcb_t *V(semaphore_t *s)
{
    if (headBlocked(s) == NULL)
    {
        *s = 1;
        return NULL;
    }
    else
    {
        pcb_t *proc = removeBlocked(s);
        enqueueReady(proc);
        decrementSoftBlockCount();
        return proc;
    }
}

#define INT_LINE(line) if(getCAUSE() & CAUSE_IP(line))

void interruptHandler(state_t *excState)
{
    excState = (state_t*)BIOSDATAPAGE;
    int cause = excState->cause;

    // Since we don't need it, we skip inter-processor interrupts (CAUSE_IP(0)

    if (cause & CAUSE_IP(1))    // Processor Local Timer
    {
        if (((getSTATUS() & STATUS_TE) >> STATUS_TE_BIT) == ON)
        {
            setTIMER(NEVER);
            memcpy(&currentProc()->p_s, excState, sizeof(state_t));
            enqueueReady(currentProc());
            schedule();
        }
    }
    if (cause & CAUSE_IP(2))    // Interval Timer (Bus)
    {
        LDIT(100000); // ACK
        while (headBlocked(getPseudoClockSem()) != NULL)
        {
            pcb_t *proc = removeBlocked(getPseudoClockSem());
            enqueueReady(proc);
            decrementSoftBlockCount();
        }
        *getPseudoClockSem() = 0;
    }
    if (cause & CAUSE_IP(3))    // Disk Devices
    {
    }
    if (cause & CAUSE_IP(4))    // Flash Devices
    {
    }
    if (cause & CAUSE_IP(5))    // Network (Ethernet) Devices
    {
    }
    if (cause & CAUSE_IP(6))    // Printer Devices
    {
        // #TODO - trasformarla in funzione, in quanto la ricerca è copia incollata per ognuna, e vedere cosa ritornare in v0


        unsigned int dev_num = 0;

        // Find the device number
        unsigned int *bitmap_addr = (unsigned int *)CDEV_BITMAP_ADDR(IL_PRINTER); // calcolated with interrupting devices bitmap
        unsigned int bitmap = *bitmap_addr;
        while (bitmap > 1 && dev_num < N_DEV_PER_IL)
        {
            ++dev_num;
            bitmap >>= 1;
        }

        // We get the device's register
        dtpreg_t *base = (dtpreg_t *)(DEV_REG_ADDR(IL_PRINTER, dev_num));

        // We get the status of the device
        unsigned int status = base->status;

        // ACK the interrupt
        base->command = ACK;

        // V the semaphore of the printer device
        semaphore_t *sem = getDevSem(EXT_IL_INDEX(IL_PRINTER)*DEVPERINT + dev_num);
        pcb_t *p = V(sem);
        if (p != NULL) // If the process still exists, we return 0 and the status
        {
            p->p_s.reg_v0 = 0;
            (p->p_savedDeviceStatus)[STATUS] = base->status;
            enqueueReady(p);
        }

    }
    if (cause & CAUSE_IP(7))    // Terminal Devices
    {
        unsigned int dev_num = 0;

        // Find the device number
        unsigned int *bitmap_addr = (unsigned int *)CDEV_BITMAP_ADDR(IL_TERMINAL); // calcolated with interrupting devices bitmap
        unsigned int bitmap = *bitmap_addr;
        while (bitmap > 1 && dev_num < N_DEV_PER_IL)
        {
            ++dev_num;
            bitmap >>= 1;
        }

        // We get the device registers
        termreg_t *base = (termreg_t *)(DEV_REG_ADDR(IL_TERMINAL, dev_num));

        // If we have transmission or reception, we ACK and V the semaphore
        // We prioritize transmission over reception
        if (base->transm_status > READY && base->transm_status != BUSY)
        {
            unsigned int status = base->transm_status & 0xFF;
            base->transm_command = ACK;
            semaphore_t *sem = getDevSem(EXT_IL_INDEX(IL_TERMINAL)*DEVPERINT + dev_num*2 + 1);
            pcb_t *p = V(sem);
            if (p != NULL)
            {
                p->p_s.reg_v0 = 0;
                (p->p_savedDeviceStatus)[STATUS] = status;
                enqueueReady(p);
            }
        }

        if (base->recv_status > READY && base->recv_status != BUSY)
        {
            unsigned int status = base->recv_status & 0xFF;
            base->recv_command = ACK;
            semaphore_t *sem = getDevSem(EXT_IL_INDEX(IL_TERMINAL)*DEVPERINT + dev_num*2);

            pcb_t *p = V(sem);
            if (p != NULL)
            {
                p->p_s.reg_v0 = 0;
                (p->p_savedDeviceStatus)[STATUS] = status;
                enqueueReady(p);
            }
        }
    }

    if (currentProc() == NULL)
    {
        schedule();
    }
    LDST(excState);
}
