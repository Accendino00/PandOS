#include "ash.h"
#include "util.h"
#include "hashtable.h"

/** Declaration of the lists */
static struct list_head semdFree_h;
static struct semd_t semdFree_table[MAXPROC];
static DEFINE_HASHTABLE(semd_h, 12);



int insertBlocked(int *semAdd, pcb_t* p){
    semd_t * sem;

    // Controlliamo se i parametri sono giusti
    if (semAdd == NULL || p == NULL)
        return true;
    if (p->p_semAdd != NULL)
        return true;

    // Cerchiamo il semaforo nella tabella hash
    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)semAdd)
    {
        // se lo troviamo aggiungiamo p alla queue dei processi bloccati associati con SEMD
        if (sem->s_key == semAdd)
        {
            list_add_tail(&p->p_list, &sem->s_procq);
            p->p_semAdd = semAdd;
            return false;
        }
    }

    // Se non lo troviamo, controlliamo se la lista dei semafori liberi è vuota
    if (list_empty(&semdFree_h))
        return true;

    // Se non è vuota, prendiamo il primo semaforo libero e lo inseriamo nella tabella hash
    sem = list_first_entry(&semdFree_h, semd_t, s_freelink);
    list_del(&sem->s_freelink);
    sem->s_key = semAdd;
    INIT_LIST_HEAD(&sem->s_procq);
    list_add_tail(&p->p_list, &sem->s_procq);
    p->p_semAdd = semAdd;
    hash_add(semd_h, &sem->s_link, (unsigned long)semAdd);
    return false;
};

pcb_t* removeBlocked(int *semAdd){
    semd_t * sem;
    pcb_t * p;

    if (semAdd == NULL)
        return NULL;

    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)semAdd)
    {
        if (sem->s_key == semAdd)
        {
            if (list_empty(&sem->s_procq))
                return NULL;
            p = list_first_entry(&sem->s_procq, pcb_t, p_list);
            list_del(&p->p_list);
            p->p_semAdd = NULL;
            if (list_empty(&sem->s_procq))
            {
                hash_del(&sem->s_link);
                list_add(&sem->s_freelink, &semdFree_h);
            }
            return p;
        }
    }
    return NULL;    
};

pcb_t* outBlocked(pcb_t *p){
    semd_t * sem;

    if (p == NULL)
        return NULL;
    if (p->p_semAdd == NULL)
        return NULL;

    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)p->p_semAdd)
    {
        if (sem->s_key == p->p_semAdd)
        {
            list_del(&p->p_list);
            p->p_semAdd = NULL;
            if (list_empty(&sem->s_procq))
            {
                hash_del(&sem->s_link);
                list_add(&sem->s_freelink, &semdFree_h);
            }
            return p;
        }
    }
    return NULL;
};

pcb_t* headBlocked(int *semAdd){
    semd_t * sem;

    if (semAdd == NULL)
        return NULL;

    hash_for_each_possible(semd_h, sem, s_link, (unsigned long)semAdd)
    {
        if (sem->s_key == semAdd)
        {
            if (list_empty(&sem->s_procq))
                return NULL;
            return list_first_entry(&sem->s_procq, pcb_t, p_list);
        }
    }
    return NULL;
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