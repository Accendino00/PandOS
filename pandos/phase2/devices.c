#include "devices.h"

#include <umps/cp0.h>
#include <umps/types.h>
#include <umps/arch.h>
#include <umps/const.h>

#define SEM_NUM ((DEVINTNUM + 1) * DEVPERINT)
HIDDEN semaphore_t dev_sems[SEM_NUM];
HIDDEN semaphore_t pseudo_clock_sem;

inline void initDevSems()
{
    for (int i = 0; i < SEM_NUM; i++)
    {
        dev_sems[i] = 0;
    }
    pseudo_clock_sem = 0;
}

inline semaphore_t *getDevSem(unsigned int index)
{
    return &(dev_sems[index]);
}

inline semaphore_t* getPseudoClockSem()
{
    return &pseudo_clock_sem;
}
