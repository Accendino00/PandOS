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


/**
 * @brief Macro that manages the device interrupt.
 * 
 * @param TYPE indicates the name of the interrupt line. 
 * It can be "IL_DISK", "IL_PRINTER",...
 * @param REGISTER TYPE is the type of "base", the struct 
 * type of the register of the device. It can be "dtpreg_t",
 * @param "..." is the list of the valid status of the device.
 * If the status isn't one of these values, we return -1 in v0.
 */
#define MANAGEDEVICE(TYPE,REGISTERTYPE,...) \
    unsigned int dev_num = 0; \
    \
    /* Find the device number */ \
    unsigned int *bitmap_addr = (unsigned int *)CDEV_BITMAP_ADDR(TYPE); \
    unsigned int bitmap = *bitmap_addr; \
    while (bitmap > 1 && dev_num < N_DEV_PER_IL) \
    { \
        ++dev_num; \
        bitmap >>= 1; \
    } \
    \
    /* We get the device's register */ \
    REGISTERTYPE *base = (REGISTERTYPE *)(DEV_REG_ADDR(TYPE, dev_num)); \
    \
    /* We get the status of the device */ \
    unsigned int status = base->status; \
    /*We search if the status is within the "..." ints*/\
    int v0_return = -1;\
    int valid_status[] = {__VA_ARGS__};\
    for(int i = 0; i < sizeof(valid_status)/sizeof(int); i++)\
    {\
        if(status == valid_status[i])\
        {\
            v0_return = 0;\
            break;\
        }\
    }\
    \
    /* ACK the interrupt */ \
    base->command = ACK; \
    \
    /* V the semaphore of the printer device */ \
    semaphore_t *sem = getDevSem(EXT_IL_INDEX(TYPE)*DEVPERINT + dev_num); \
    pcb_t *p = V(sem); \
    if (p != NULL) /* If the process still exists, we return 0 and the status */ \
    { \
        p->p_s.reg_v0 = v0_return; \
        (p->p_savedDeviceStatus)[STATUS] = status; \
        enqueueReady(p); \
    }

/**
 * @brief Calculates the device number of the interrupting device.
 * 
 * @param line The interrupt line.
 * 
 * @return The device number of the interrupting device.
*/
static inline int getInterruptingDevice(unsigned int line) {
    unsigned int dev_num = 0;

    // Find the device number
    unsigned int *bitmap_addr = (unsigned int *)CDEV_BITMAP_ADDR(line); // calcolated with interrupting devices bitmap
    unsigned int bitmap = *bitmap_addr;
    while (bitmap > 1 && dev_num < N_DEV_PER_IL)
    {
        ++dev_num;
        bitmap >>= 1;
    }

    return dev_num;
}


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

#define INT_LINE(line) getCAUSE() & CAUSE_IP(line)

void interruptHandler(state_t *excState)
{
    // Since we don't need it, we skip inter-processor interrupts (CAUSE_IP(0))

    // We use "if"s, so that if we have more than one interrupt pending, we handle them all

    if (INT_LINE(1))    // Processor Local Timer
    {
        // If the TE bit is set, we handle the interrupt
        if (((getSTATUS() & STATUS_TE) >> STATUS_TE_BIT))
        {
            setTIMER(NEVER);
            memcpy(&currentProc()->p_s, excState, sizeof(state_t));
            enqueueReady(currentProc());
        }
    }
    
    if (INT_LINE(2))    // Interval Timer (Bus)
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
    if (INT_LINE(DISKINT))     // Disk Devices - 3
    {
        MANAGEDEVICE(IL_DISK, dtpreg_t, 0,1,2,3,4,5,6);
    }
    if (INT_LINE(FLASHINT))    // Flash Devices - 4
    {
        MANAGEDEVICE(IL_FLASH, dtpreg_t, 0,1,2,3,4,5,6,7);
    }
    if (INT_LINE(NETWINT))     // Network (Ethernet) Devices - 5
    {
        MANAGEDEVICE(IL_ETHERNET, dtpreg_t, 0,1,2,3,4,5,6,7,128);
    }
    if (INT_LINE(PRNTINT))    // Printer Devices - 6
    {
        MANAGEDEVICE(IL_PRINTER, dtpreg_t, 0,1,2,3,4);
    }
    if (INT_LINE(TERMINT))    // Terminal Devices - 7
    {
        // Since we need to treat terminals a bit differently than other devices,
        // we don't use the macro in this function.

        unsigned int dev_num = getInterruptingDevice(IL_TERMINAL);

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
        }/* else if(!(base->transm_status >= 0 && base->transm_status <= 5)) // Ovvero non è nel range
            // ACK e si rimuove il blocco del semaforo
            // Si mette in v0 il valore -1

            #TODO - verificare se questa può andare bene come soluzione
        */

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
