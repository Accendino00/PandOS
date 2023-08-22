#include <umps/libumps.h>
#include <pandos_const.h>
#include <pandos_types.h>
#include "pcb.h"
#include "ash.h"
#include "ns.h"
#include "scheduler.h"
#include "exception.h"
#include "print.h"
#include "processor.h"

/** Defined somewhere else (p2test_04.c)*/
extern void uTLB_RefillHandler();

/**
 * @brief Initializes the Pandos kernel.
 * 
 * This function inizializes the pass up vector, the data structures used by
 * the kernel and the scheduler.
*/
void init_kernel() {
    init_passupvector((memaddr)uTLB_RefillHandler, (memaddr)exception_handler);
    initPcbs();
    initASH();
    initNamespaces();
    init_scheduler();
    PRINT_DEBUG("Kernel initialized\n");
}

state_t second_state;
#define LOOPNUM 10000
#define MINLOOPTIME 30000

void test3()
{
    PRINT_DEBUG("Test process 2\n");

    

    SYSCALL(TERMPROCESS, 0, 0, 0);
}



void test2() {
    PRINT_DEBUG("Test process\n");

    set_state(&second_state);
    second_state.pc_epc = second_state.reg_t9 = (memaddr)test3;

    int pid2 = SYSCALL(CREATEPROCESS,(int)&second_state, 0, 0);

    cpu_t now1, now2;
    cpu_t cpu_t1, cpu_t2;

    STCK(now1);                         /* time of day   */
    cpu_t1 = SYSCALL(GETTIME, 0, 0, 0); /* CPU time used */
    PRINT_DEBUG("Accumulated time by p2: %d\n", cpu_t1);

    int i;
    /* delay for several milliseconds */
    for (i = 1; i < LOOPNUM; i++)
        ;

    cpu_t2 = SYSCALL(GETTIME, 0, 0, 0); /* CPU time used */
    PRINT_DEBUG("Accumulated time by p2 (again): %d\n", cpu_t2);
    STCK(now2); /* time of day  */

    if (((now2 - now1) >= (cpu_t2 - cpu_t1)) &&
        ((cpu_t2 - cpu_t1) >= (MINLOOPTIME / (*((cpu_t *)TIMESCALEADDR)))))
    {
        PRINT_DEBUG("p2 is OK\n");
    }
    else
    {
        if ((now2 - now1) < (cpu_t2 - cpu_t1))
            PRINT_DEBUG("error: more cpu time than real time\n");
        if ((cpu_t2 - cpu_t1) < (MINLOOPTIME / (*((cpu_t *)TIMESCALEADDR))))
            PRINT_DEBUG("error: not enough cpu time went by (%d)\n", cpu_t2 - cpu_t1);
        halt("p2 blew it!\n");
    }

    PRINT_DEBUG("Created process %d\n", pid2);

    SYSCALL(TERMPROCESS, 0, 0, 0);
}

/** Defined somewhere else (p2test_04.c)*/
extern void test();

/** PANDOS ENTRY POINT */
int main() {
    init_kernel();
    init_process((memaddr)test2);
    schedule(NULL, SCH_DO_NOTHING);

    return 0;
}