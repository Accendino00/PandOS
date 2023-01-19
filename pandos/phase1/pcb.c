#include "pcb.h"
#include "util.h"

/** Declaration of the lists */
static struct list_head pcbFree_h;
static struct pcb_t pcbFree_table[MAXPROC];

// PCBs allocation

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

void freePcb(struct pcb_t *p)
{
    /* Check if p is NULL */
    if (p == NULL)
        return;

    list_add_tail(&p->p_list, &pcbFree_h);
}

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


// PBCs lists

void mkEmptyProcQ(struct list_head *head)
{
    if (head == NULL)
        return;
    INIT_LIST_HEAD(head);
}

int emptyProcQ(struct list_head *head)
{
    if (head == NULL)
        return true;
    return list_empty(head);
}


void insertProcQ(struct list_head *head, pcb_t *p)
{
    list_add_tail(&p->p_list, head);
};


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

// PCBs trees

int emptyChild(pcb_t *p){

};

void insertChild(pcb_t *prnt, pcb_t *p){

};

pcb_t *removeChild(pcb_t *p){

};

pcb_t *outChild(pcb_t *p){

};
