#include "ash.h"
#include "util.h"
#include "hashtable.h"

/** Declaration of the lists */
static struct semd_t* semdFree_h;
static struct semd_t semdFree_table[MAXPROC];
static DEFINE_HASHTABLE(semd_h, 12);

int insertBlocked(int *semAdd, pcb_t* p){
    
   //cerca il semaforo nell'ASH
   // se c'è inserisci il pcb
   //altrimenti alloca il semaforo nell'ASH
   // se non ci sono semafori liberi ritorna TRUE
   //ritorna FALSE

//     if (list_empty(&semdFree_h)) {
//         return TRUE;
//     } else {
//         semd_t* s = findSEMD(semd_h, semAdd);

//         if (s != NULL) {
//             p->p_semAdd = semAdd;
//             list_add_tail(&p->p_list, &s->s_procq);
//         } else {
//             hash_add(semd_h, , semAdd);
//         }

        
//     }

//    return FALSE;

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