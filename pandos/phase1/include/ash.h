#ifndef ASH
#define ASH

#include "types.h"
#include "pandos_const.h"
#include "pandos_types.h"


/**
 * @brief Inserts the PCB pointed by p in the queue of blocked processes associated with the SEMD with key semAdd.
 * 
 * If the semaphore isn't in that ASH, the function allocates a new SEMD from the free list and inserts it into ASH,
 * setting the fields key and s_procQ.
 * 
 * @param semAdd The SEMD you want to add the process p to.
 * @param p The process you want to add to the SEMD.
 * @return  TRUE if it isn't possible to allocate a new SEMD, FALSE otherwise.
 */
int insertBlocked(int *semAdd, pcb_t* p);

/**
 * @brief Returns the first PCB from the queue of blocked processes associated with the SEMD with key semAdd.
 *
 * If the queue of blocked processes becomes empty, the function removes the descriptor from the ASH e it inserts
 * it in the queue of free descriptors.
 *
 * @param semAdd The semaphore you want to remove the first blocked process from.
 * @return If the descriptor doesn't exist in the ASH, the function returns NULL. Otherwise, it returns the removed element.
 */
pcb_t* removeBlocked(int *semAdd);

/**
 * @brief Removes the PCB pointed by p from the queue of the semaphore it is blocked on.
 *
 * If the queue of blocked processes becomes empty, the function removes the descriptor from the ASH e it inserts
 * it in the queue of free descriptors.
 * 
 * @param p The PCB you want to remove.
 * @return NULL if the PCB doesn't appear on the queue of the semaphore, otherwise it returns p.
 */
pcb_t* outBlocked(pcb_t *p);

/**
 * @brief Returns without removing the pointer to the PCB that resides in the head of the queue of processes
 * associated with the SEMD with key semAdd.
 * 
 * @param semAdd The SEMD you want to retrieve the first PCB from.
 * @return NULL if the SEMD doesn't appear in the ASH or if it's queue is empty, otherwise the pointer to the first PCB.
 */
pcb_t* headBlocked(int *semAdd);

/**
 * @brief Initializes the list of semdFree so that it contains all the elements in the semdTable.
 * 
 * This method is called only once during the initialization of the data structure.
 * 
 */
void initASH();

pcb_t *findPcb(int pid, struct list_head *list);

void getBlockedProcesses();

#endif