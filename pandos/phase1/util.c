#include "util.h"

struct pcb_t* resetPcb(struct pcb_t* p) {
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

    return p;
}