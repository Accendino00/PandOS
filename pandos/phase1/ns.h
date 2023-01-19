#ifndef NS
#define NS

#include "pandos_const.h"
#include "pandos_types.h"
#include "types.h"

/**
 * @brief Initializes the list of free namespaces.
 * 
 * This method is called only once during the initialization of the data structure.
 * 
 */
void initNamespaces();

/**
 * @param p The process from which you want to return the namespace of.
 * @param type The type of the namespace.
 * @return The namespace of type type associated to the process p, NULL otherwise.
 */
nsd_t* getNamespace(pcb_t* p, int type);

/**
 * @brief Associates the namespace ns to the process p and to all of his children.
 * 
 * @param p The process you want to be associated with the namespace ns.
 * @param ns The namespace you want to associate the process p to.
 * @return FALSE if there is an error, TRUE otherwise.
 */
int addNamespace(pcb_t *p, nsd_t* ns);

/**
 * @brief Allocates a namespace of type type from the correct list.
 * 
 * @param type The type of the namespace you want to allocate.
 * @return nsd_t* The newly allocated namespace.
 */
nsd_t* allocNamespace(int type);

/**
 * @brief Frees the namespace ns re-inserting it into the correct namespaces list.
 * 
 * @param ns The namespace you want to free.
 */
void freeNamespace(nsd_t *ns);

#endif