#include "ash.h"
#include "util.h"
#include "hashtable.h"
#include "pcb.h"
#include "print.h"
#include "devices.h"

/** Declaration of the lists */
HIDDEN volatile struct list_head semdFree_h;
HIDDEN volatile struct semd_t semdFree_table[MAXPROC];
HIDDEN volatile DEFINE_HASHTABLE(semd_h, 12);

int insertBlocked(int *semAdd, pcb_t *p)
{
    semd_t *sem;

    // We first check if the parameters are NULL
    if (semAdd == NULL || p == NULL)
        return 1;
    if (p->p_semAdd != NULL)
        return 2;

    // We search for the semaphore in the hash table
    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)semAdd)
    {
        // If we do find the semaphore, we add p to the queue of processes blocked associated with this semaphore
        if (sem->s_key == semAdd)
        {
            list_add_tail(&p->p_list, &sem->s_procq);
            p->p_semAdd = semAdd;
            return FALSE;
        }
    }

    // If we do not find it, we check if the list of free semaphores has elements
    if (list_empty((struct list_head *)&semdFree_h))
        return 3;

    // If the list of free semaphores isn't empty, then we take the first
    // semaphore, we set its values, and we insert it in the tail of the hash table
    sem = list_first_entry(&semdFree_h, semd_t, s_freelink);
    list_del(&sem->s_freelink);

    sem->s_key = semAdd;
    INIT_LIST_HEAD(&sem->s_procq);

    list_add_tail(&p->p_list, &sem->s_procq);
    p->p_semAdd = semAdd;
    hash_add(semd_h, &sem->s_link, (unsigned long)semAdd);

    return FALSE;
};

pcb_t *removeBlocked(int *semAdd)
{
    semd_t *sem;
    pcb_t *p;

    // We first check if the parameters are NULL
    if (semAdd == NULL)
        return NULL;

    // We search for the semaphore in the hash table
    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)semAdd)
    {
        // If we do find the semaphore
        if (sem->s_key == semAdd)
        {
            // We remove the first process from the queue of processes
            // blocked associated with this semaphore
            if (list_empty(&sem->s_procq))
                return NULL;
            p = list_first_entry(&sem->s_procq, pcb_t, p_list);
            list_del(&p->p_list);
            p->p_semAdd = NULL;

            // If the queue of processes blocked associated with this semaphore is empty,
            // we remove the semaphore from the hash table and we add it to the list of free semaphores
            if (list_empty(&sem->s_procq))
            {
                hash_del(&sem->s_link);
                list_add(&sem->s_freelink, (struct list_head *)&semdFree_h);
            }
            return p;
        }
    }
    return NULL;
};

pcb_t *outBlocked(pcb_t *p)
{
    semd_t *sem;

    // We first check if the parameters are NULL
    if (p == NULL)
        return NULL;
    if (p->p_semAdd == NULL)
        return NULL;

    // We search for the semaphore in the hash table
    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)p->p_semAdd)
    {
        // If we do find the semaphore where p is blocked in
        if (sem->s_key == p->p_semAdd)
        {
            // We remove the process from the queue of processes
            // blocked associated with this semaphore
            list_del(&p->p_list);
            p->p_semAdd = NULL;

            // If the queue of processes blocked associated with this semaphore is empty,
            // we remove the semaphore from the hash table and we add it to the list of free semaphores
            if (list_empty(&sem->s_procq))
            {
                hash_del(&sem->s_link);
                list_add(&sem->s_freelink, (struct list_head *)&semdFree_h);
            }

            return p;
        }
    }
    // If we don't find the semaphore where p is blocked in, we return NULL
    return NULL;
};

pcb_t *headBlocked(int *semAdd)
{
    semd_t *sem;

    // Check for NULL parameters
    if (semAdd == NULL)
        return NULL;

    // Search for the semaphore in the hash table
    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)semAdd)
    {
        // If we do find the semaphore
        if (sem->s_key == semAdd)
        {
            // If the queue of processes blocked associated with this semaphore is empty,
            // we return NULL
            if (list_empty(&sem->s_procq))
                return NULL;
            // Else, we return the first process from the queue of processes
            // blocked associated with this semaphore
            return list_first_entry(&sem->s_procq, pcb_t, p_list);
        }
    }
    return NULL;
};

void initASH()
{
    int i;

    // We initialize the lists
    INIT_LIST_HEAD((struct list_head *)&semdFree_h);
    hash_init(semd_h);

    // We add all the semaphores to the list of free semaphores
    for (i = 0; i < MAXPROC; i++)
    {
        list_add(&semdFree_table[i].s_freelink, &semdFree_h);
    }
};

pcb_t *findPcb(int pid, struct list_head *list)
{
    pcb_t *p;

    p = searchPcbByPid(pid, list);

    if (p == NULL)
    {
        int btk = 0;
        semd_t *it;
        hash_for_each(semd_h, btk, it, s_link)
        {
            p = searchPcbByPid(pid, &it->s_procq);
            if (p != NULL)
            {
                return p;
            }
        }
    }
    return p;
}

void getBlockedProcesses()
{
    int btk = 0;
    semd_t *it;
    pcb_t *p;
    hash_for_each(semd_h, btk, it, s_link)
    {
        if ((memaddr)it->s_key >= (memaddr)getDevSem(0) && (memaddr)it->s_key <= (memaddr)getDevSem(47))
        {
            int index = ((int)it->s_key - (int)getDevSem(0)) / sizeof(unsigned int);
            printf("Processes blocked on line %d, device %d: ", index / 8 + 3, index % 8);
            list_for_each_entry(p, &it->s_procq, p_list)
            {
                printf("%d\n", p->p_pid);
            }
        }
        else
        {
            list_for_each_entry(p, &it->s_procq, p_list)
            {
                printf("Process %d is blocked on %p\n", p->p_pid, it->s_key);
            }
        }
    }
}