#ifndef UTILS
#define UTILS

#include "types.h"
#include "pandos_types.h"
#include "pandos_const.h"

/**
 * @brief Resets the PCB pointed by p.
 *
 * @param p the PCB to reset.
 * @return The resetted PCB.
 */
struct pcb_t* resetPcb(struct pcb_t* p);

#endif