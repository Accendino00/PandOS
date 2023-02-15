#include "ash.h"
#include "util.h"
#include "hashtable.h"

/** Declaration of the lists */
static struct list_head semdFree_h;
static struct semd_t semdFree_table[MAXPROC];
static DEFINE_HASHTABLE(semd_h, 12);

int insertBlocked(int *semAdd, pcb_t* p){
    semd_t *sem;

    if (list_empty(&semdFree_h) || p == NULL || semAdd == NULL || p->p_semAdd != NULL)
    {
        return TRUE;
    }
    else
    {
        


        return FALSE;
    }
};

pcb_t* removeBlocked(int *semAdd){
    
};

pcb_t* outBlocked(pcb_t *p){

};

pcb_t* headBlocked(int *semAdd){

};

void initASH(){
    
    int i;

    INIT_LIST_HEAD(&semdFree_h);
    hash_init(semd_h);

    for (i = 0; i < MAXPROC; i++)
    {
        list_add(&semdFree_table[i].s_freelink, &semdFree_h);
    }
};