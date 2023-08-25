#ifndef UTILS
#define UTILS

#include "types.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include "list.h"

/** PCBs HELPER FUNCTIONS */

/**
 * @brief Resets the PCB pointed by p.
 *
 * @param p the PCB to reset.
 * @return The resetted PCB.
 */
struct pcb_t *resetPcb(struct pcb_t *p);


/* LISTs HELPER FUNCTIONS */

/**
 * @brief Safely deletes a node from a list.
 *
 */
static inline void list_safe_del(struct list_head *node)
{
    if (!(node->next == NULL && node->prev == NULL))
    {
        list_del(node);
        node->next = node->prev = NULL;
    }
}

/**
 * @brief Searches for an item in a list.
 * 
 * @param item The item to search for.
 * @param head The list to search in.
 * @param cmp The comparison function to determine wheter or not two items are equal.
 * @return The item if found, NULL otherwise.
 */
static inline const struct list_head *list_search(const struct list_head *item, const struct list_head *head, int (*cmp)(const struct list_head *, const struct list_head *))
{
    struct list_head *tmp;
    for(tmp = head->next; tmp != head; tmp = tmp->next) {
        if (!cmp(item, tmp))
            return tmp;
    }
    return NULL;
}

/**
 * @brief Checks if two items of a list are equal.
 * 
 * @param a The first item.
 * @param b The second item.
 * @return Returns 0 if the items are equal, 1 otherwise.
 */
static inline bool list_cmp(const struct list_head *a, const struct list_head *b)
{
    return a != b;
}

/**
 * @brief Checks if an item is contained in a list.
 * 
 * @param item The item to search for.
 * @param head The list to search in.
 * @return TRUE if the item is contained in the list, FALSE otherwise. 
 */
static inline int list_contains(const struct list_head *item, const struct list_head *head)
{
    return list_search(item, head, list_cmp) != NULL;
}

static inline int list_null(const struct list_head *head)
{
    return head->prev == NULL && head->next == NULL;
}

#endif