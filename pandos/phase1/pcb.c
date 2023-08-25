#include "pcb.h"
#include "util.h"

/** Declaration of the lists */
static volatile struct list_head pcbFree_h;
static volatile struct pcb_t pcbFree_table[MAXPROC];

pcb_t* get_pcb_table() {
    return pcbFree_table;
}
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

    list_safe_del(&p->p_list);
    list_add(&p->p_list, &pcbFree_h);
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
        // If I do have free PCBs, I take the first one and I remove it from the list
        pcb_t *first = container_of(pcbFree_h.next, pcb_t, p_list);
        list_safe_del(pcbFree_h.next);
        return resetPcb(first);
    }
}

// PBCs lists

void mkEmptyProcQ(struct list_head *head)
{
    if (list_empty(head))
        return;
    INIT_LIST_HEAD(head);
}

int emptyProcQ(struct list_head *head)
{
    if (head == NULL)
        return TRUE;

    return list_empty(head);
}

void insertProcQ(struct list_head *head, pcb_t *p)
{
    if (p == NULL || head == NULL)
        return;

    list_safe_del(&p->p_list);
    list_add_tail(&p->p_list, head);
};

pcb_t *headProcQ(struct list_head *head)
{
    if (list_empty(head) || head == NULL)
    {
        return NULL;
    }

    return container_of(list_next(head), pcb_t, p_list);
};

pcb_t *removeProcQ(struct list_head *head)
{
    if (list_empty(head) || head == NULL)
    {
        return NULL;
    };

    pcb_t *toremove = container_of(list_next(head), pcb_t, p_list);
    
    return outProcQ(head, toremove);
};

pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{
    if (!list_contains(&p->p_list, head) || head == NULL || p == NULL)
        return NULL;

    list_safe_del(&p->p_list);
    return p;
};

// PCBs trees

int emptyChild(pcb_t *p)
{
    // If p is null, we choose to return false
    if (p == NULL)
        return FALSE;

    return list_empty(&p->p_child);
};

void insertChild(pcb_t *prnt, pcb_t *p)
{
    if (prnt == NULL || p == NULL)
        return;

    p->p_parent = prnt;
    list_add_tail(&p->p_sib, &prnt->p_child);
};

pcb_t *removeChild(pcb_t *p)
{
    if (emptyChild(p) || p == NULL)
        return NULL;

    return outChild(container_of(list_next(&p->p_child), pcb_t, p_sib));
};

pcb_t *outChild(pcb_t *p)
{
    if (p == NULL || p->p_parent == NULL || list_empty(&p->p_parent->p_child))
        return NULL;

    list_del(&p->p_sib);
    p->p_parent = NULL;
    return p;
};
