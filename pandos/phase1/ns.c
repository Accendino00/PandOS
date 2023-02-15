#include "ns.h"
#include "util.h"

static volatile nsd_t PID_nsd[MAXPROC];
static volatile struct list_head PID_nsFree_h;
static volatile struct list_head PID_nsList_h;


void initNamespaces() {
    int i;
    INIT_LIST_HEAD(&PID_nsFree_h);
    INIT_LIST_HEAD(&PID_nsList_h);
    for (i = 0; i < MAXPROC; i++) {
        INIT_LIST_HEAD(&PID_nsd[i].n_link);
        list_add_tail(&PID_nsd[i].n_link, &PID_nsFree_h);
    }
};

nsd_t* getNamespace(pcb_t* p, int type){
    return p->namespaces[type];
};

int addNamespace(pcb_t *p, nsd_t* ns){
    if(ns == NULL || p == NULL){
        return false;
    }
    p->namespaces[ns->n_type] = ns;
    pcb_t *child;
    list_for_each_entry(child, &p->p_child, p_sib){
        child->namespaces[ns->n_type] = ns;
    }

    list_safe_del(&ns->n_link);
    list_add_tail(&ns->n_link, &PID_nsList_h);
    return true;
};

nsd_t* allocNamespace(int type){
    nsd_t *ns;
    if(list_empty(&PID_nsFree_h)){
        return NULL;
    }
    ns = list_first_entry(&PID_nsFree_h, nsd_t, n_link);
    list_del(&ns->n_link);
    ns->n_type = type;
    return ns;
};

void freeNamespace(nsd_t *ns){
    list_safe_del(&ns->n_link);
    list_add_tail(&ns->n_link, &PID_nsFree_h);
};