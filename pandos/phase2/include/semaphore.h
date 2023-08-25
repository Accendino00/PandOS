#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "scheduler.h"
#include "ash.h"
#include "util.h"

typedef unsigned int semaphore_t;

/**
 * @brief Performs a P operation on the semaphore.
 *
 * @param s Pointer to the semaphore.
 *
 * @return A scheduler control struct.
 */
static inline scheduler_control_t P(semaphore_t *s, pcb_t *p)
{
    pcb_t *t;
    int r;

    if (*s == 0) // we must wait
    {
        if (!list_null(&p->p_list)) // remove the pcb from any list
            dequeue_ready(p);

        if ((r = insertBlocked(s, p)) > 0) // insert the pcb in the blocked list
        {
            panic("P: insertBlocked failed (%d)\n", r);
        }

        increment_softblocked_count();
        return (scheduler_control_t){NULL, SCH_BLOCK}; // block the process
    }
    else if ((t = removeBlocked(s)) != NULL) // there is a process to unblock
    {
        enqueue_ready(t);
        decrement_softblocked_count();
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
    }
    else
    {
        --*s;
    }

    return (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
}

/**
 * @brief Performs a V operation on the semaphore.
 *
 * @param s Pointer to the semaphore.
 *
 * @return The newly unblocked process.
 */
static inline pcb_t *V(semaphore_t *s)
{
    pcb_t *t;
    pcb_t *p = get_current_process();
    int r;

    if (*s == 1)
    {
        if (p != NULL && p->p_semAdd == NULL)
        {                               // if calling process isn't null and it isn't blocked already
            if (!list_null(&p->p_list)) // remove the pcb from any list
                dequeue_ready(p);

            increment_softblocked_count();
            if ((r = insertBlocked((int *)s, p)) > 0) // insert the pcb in the blocked list
                panic("V: insertBlocked failed (%d)\n", r);
        }

        return NULL;
    }

    else if ((t = removeBlocked(s)) != NULL)
    {
        decrement_softblocked_count();
        enqueue_ready(t);
        return t;
    }
    else
    {
        ++*s;
        return p;
    }
}

#endif