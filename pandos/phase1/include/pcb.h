#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "list.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include "container_of.h"

/**
 * @brief Initializes the pcbFree list.
 * 
 * This method is callong once when the data structure is first initialized.
 */
void initPcbs();

/**
 * @brief Inserts the PCB pointed by p in the list of free PCBs.
 * 
 * @param p The PCB to be inserted
 */
void freePcb(pcb_t *p);

/**
 * @brief Returns a free PCB to be used, if there is one.
 * 
 * @return NULL if pcbFree_h is empty, otherwhise a PCB from the same list.
 */
pcb_t *allocPcb();

/**
 * @brief Creates an empty list of PCBs.
 * 
 * @param head The list of PCBs to be created.
 */
void mkEmptyProcQ(struct list_head *head);

/**
 * @brief Checks whether or not a process list is empty.
 * 
 * @param head The list to be checked.
 * @return TRUE if the list is empty, FALSE otherwise.
 */
int emptyProcQ(struct list_head *head);

/**
 * @brief Inserts the element pointed by p in the list of processes pointed by head.
 * 
 * @param head The list in which you want to insert the process.
 * @param p The process you want to insert in the list pointed by head.
 */
void insertProcQ(struct list_head *head, pcb_t *p);

/**
 * @brief Returns the head element of the list of processes pointed by head, without removing it.
 * 
 * @param head The list from which you want to get the head.
 * @return NULL if the head is empty, otherwise the first element of the list pointed by head.
 */
pcb_t *headProcQ(struct list_head *head);

/**
 * @brief Removes the first element of the list of processes pointed by head.
 *
 * @param head The list from which you want to remove the first element.
 * @return NULL if the empty is empty, otherwise the first element of the list pointed by head.
 */
pcb_t *removeProcQ(struct list_head *head);

/**
 * @brief Removes the process p from the list of processes pointed by head.
 *
 * @param head The list from which you want to remove the process p from.
 * @param p The process to be removed from the list.
 * @return NULL if the empty is empty, otherwise a pointer to the removed element.
 */
pcb_t *outProcQ(struct list_head *head, pcb_t *p);

/**
 * @param p The process you want to check.
 * @return TRUE if the PCB pointed by p has no children, FALSE otherwise.
 */
int emptyChild(pcb_t *p);

/**
 * @brief Inserts the PCB pointed by p as a children of the PCB pointed by prnt.
 * 
 * @param prnt The PCB you want to add a child process to.
 * @param p The PCB to be added to prnt as a child.
 */
void insertChild(pcb_t *prnt, pcb_t *p);

/**
 * @brief Removes the first child of the PCB pointed by p.
 * 
 * @param p The PCB you want to remove the first child from.
 * @return NULL if p has no children, otherwise the pointer of the removed PCB.
 */
pcb_t *removeChild(pcb_t *p);

/**
 * @brief Removes the PCB pointed by p from it's father's list of children PCBs.
 * 
 * @param p The PCB to be removed.
 * @return NULL if p has no father, otherwise the same pointer p.
 */
pcb_t *outChild(pcb_t *p);

#endif