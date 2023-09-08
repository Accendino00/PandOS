#include "ns.h"
#include "util.h"

HIDDEN volatile nsd_t PID_nsd[MAXPROC];
HIDDEN volatile struct list_head PID_nsFree_h;
HIDDEN volatile struct list_head PID_nsList_h;


void initNamespaces() {
    int i;
    INIT_LIST_HEAD((struct list_head*) &PID_nsFree_h);
    INIT_LIST_HEAD((struct list_head*) &PID_nsList_h);

    for (i = 0; i < MAXPROC; i++) {
        // Initializing the namespaces of the table
        INIT_LIST_HEAD(&PID_nsd[i].n_link);
        // We add these to the list of free namespaces
        list_add_tail(&PID_nsd[i].n_link, &PID_nsFree_h);
    }
};

nsd_t* getNamespace(pcb_t* p, int type){
    // if(p == NULL){
    //     return NULL;
    // }
    // return p->namespaces[type];
    for (int i = 0; i < NS_TYPE_MAX; i++)
    {
        if (p->namespaces[i]->n_type == type)
        {
            return p->namespaces[i];
        }
    }
    return NULL;
};


void setAllChildNamespace(struct list_head * p, nsd_t* ns) {
    // For sibling of p, set all their ns to ns and then call this recursively for all childs
    pcb_t *child;
    list_for_each_entry(child, p, p_sib){
        child->namespaces[ns->n_type] = ns;
        setAllChildNamespace(&child->p_child, ns);
    } 
}

int addNamespace(pcb_t *p, nsd_t* ns){
    // If param is null, return error (false)
    if(ns == NULL || p == NULL){
        return FALSE;
    }

    // We set the namespace of the process and all of his children
    p->namespaces[ns->n_type] = ns;
    setAllChildNamespace(&p->p_child, ns);

    // We remove the namespace from the free list and add it 
    // to the list of used namespaces
    list_safe_del(&ns->n_link);
    list_add_tail(&ns->n_link, &PID_nsList_h);

    return TRUE;
};

nsd_t* allocNamespace(int type){
    nsd_t *ns;
    
    // Check if there are free namespaces
    if(list_empty(&PID_nsFree_h)){
        return NULL;
    }

    // Get the first free namespace and set its type
    ns = list_first_entry(&PID_nsFree_h, nsd_t, n_link);
    list_del(&ns->n_link);
    ns->n_type = type;

    return ns;
};

void freeNamespace(nsd_t *ns){
    // We remove the namespace from the list of used namespaces
    // and add it to the list of free namespaces
    list_safe_del(&ns->n_link);
    list_add_tail(&ns->n_link, &PID_nsFree_h);
};