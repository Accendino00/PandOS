#include "pcb.h"

/** Declaration of the lists */
struct list_head pcbFree_h;
struct pcb_t pcbFree_table[MAXPROC];

/**
 * @brief Resets the pcb pointed by p
 * 
 * @param p the pcb to reset
 */
static inline struct pcb_t* resetPcb(struct pcb_t *p)
{
    if (p == NULL)
        return NULL;

    p->p_list.next = NULL;
    p->p_list.prev = NULL;

    INIT_LIST_HEAD(&p->p_child);

    p->p_parent = NULL;
    p->p_sib.next = NULL;
    p->p_sib.prev = NULL;

    p->p_s.entry_hi = 0;
    p->p_s.cause = 0;
    p->p_s.status = 0;
    p->p_s.pc_epc = 0;
    p->p_s.hi = 0;
    p->p_s.lo = 0;

    for (int i = 0; i < STATE_GPR_LEN; i++)
    {
        p->p_s.gpr[i] = 0;
    }

    p->p_time = 0;
    p->p_semAdd = NULL;
}

/**
 * @brief
 *  Initialize the pcbFree list to contain all the elements of the
 *  static array of MAXPROC pcbs. This method will be called only
 *  once during data structure initialization.
 */
void initPcbs()
{
    /* Initialize the list */
    INIT_LIST_HEAD(&pcbFree_h);

    /* Add pcb_table elements to the list */
    for (int i = 0; i < MAXPROC; i++)
    {
        list_add(&pcbFree_table[i].p_list, &pcbFree_h);
    }
}

/**
 * @brief
 *  Insert the element pointed to by p onto the pcbFree list.
 *
 * @param p element to be inserted in the pcbFree list
 */
void freePcb(struct pcb_t *p)
{
    /* Check if p is NULL */
    if (p == NULL)
        return;

    list_add_tail(&p->p_list, &pcbFree_h);
}

/**
 * @brief 
 *  Return NULL if the pcbFree list is empty. Otherwise, remove
 *  an element from the pcbFree list, provide initial values
 *  for ALL of the pcbs fields (i.e. NULL and/or 0) and then
 *  return a pointer to the removed element. pcbs get reused, so
 *  it is important that no previous value persist in a pcb when
 *  it gets reallocated.
 *
 * @return pcb_t*
 */
pcb_t *allocPcb()
{
    /* Check if i have pcb in the list of free ones which i can allocate */
    if (list_empty(&pcbFree_h))
    {
        return NULL;
    }
    else
    {
        pcb_t *first = list_first_entry(pcbFree_h.next, pcb_t, p_list);
        list_del(pcbFree_h.next);

        return resetPcb(first);
    }
}

/**
 * @brief
 *  This method is used to initialize a variable to be tail pointer
 *  to a process queue.
 *  Return a pointer to the tail of an empty process queue; i.e. NULL
 *
 * @param head
 */
void mkEmptyProcQ(struct list_head *head)
{
    if (head == NULL)
        return;
    INIT_LIST_HEAD(head);
}

/**
 * @brief
 *  Return TRUE if the queue whose tail is pointed to by tp is
 *  empty. Return FALSE otherwise.
 *
 * @param head Pointer to the head of the list to check
 * @return int Boolean value of "true" if the list is empty, "false" otherwis
 */
int emptyProcQ(struct list_head *head)
{
    if (head == NULL)
        return true;
    return list_empty(head);
}

/**
 * @brief
 *  Insert the pcb pointed to by p into the process queue whose tail
 *  pointer is pointed to by tp. Note the double indirection through
 *  tp to allow for the possible updating of the tail pointer as well.
 *
 * @param head
 * @param p
 */
void insertProcQ(struct list_head *head, pcb_t *p)
{
    list_add_tail(&p->p_list, head);
};

/**
 * @brief
 *  Return a pointer to the first pcb from the process queue whose
 *  tail is pointed to by tp. Do not remove this pcbfrom the process
 *  queue. Return NULL if the process queue is empty.
 *
 * @param head
 * @return pcb_t*
 */
pcb_t *headProcQ(struct list_head *head)
{
    if (head == NULL)
    {
        return NULL;
    }

    return container_of(head, pcb_t, p_list);
};

pcb_t *removeProcQ(struct list_head *head)
{
    if (head == NULL)
    {
        return NULL;
    };
    __list_del_entry(head);
    return head;
};

pcb_t *outProcQ(struct list_head *head, pcb_t *p){
    struct p_list *tmp = head;
    if(head==NULL){
        return NULL;
    }
    while(tmp!=NULL){
        if(container_of(head, pcb_t, p_list)==p){
            removeProcQ(p);
        }
        else{
            tmp=tmp->next;
        }  
    }
};

int emptyChild(pcb_t *p){

};

void insertChild(pcb_t *prnt, pcb_t *p){

};

pcb_t *removeChild(pcb_t *p){

};

pcb_t *outChild(pcb_t *p){

};
