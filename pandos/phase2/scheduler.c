#include <scheduler.h>
#include <pcb.h>

#include <umps/types.h>
#include <umps/cp0.h>

#include <print.h>
#include <processor.h>
#include <ash.h>

static volatile unsigned int process_count = 0;
static volatile unsigned int soft_block_count = 0;
static volatile pcb_t *current_active_process = NULL;
static volatile struct list_head ready_queue;

static volatile unsigned int pid_counter = 0;
static volatile cpu_t start_time = 0;

static inline unsigned int get_new_pid()
{
    return pid_counter++;
}

void init_scheduler()
{
    process_count = 0;
    soft_block_count = 0;
    current_active_process = NULL;
    pid_counter = 0;
    store_tod(&start_time);
    reset_interval_timer();
    reset_plt();
    mkEmptyProcQ(&ready_queue);
}

void init_process(memaddr pc)
{
    pcb_t *new = create_process();
    if (new == NULL)
        halt("Error while initializing entry point!");
    new->p_s.pc_epc = new->p_s.reg_t9 = pc;
    RAMTOP(new->p_s.reg_sp);
    new->p_s.status = new->p_s.status;

    PRINT_DEBUG("Process %d created\n", new->p_pid);
    return;
}

void enqueue_ready(pcb_t *process)
{
    if (process == NULL)
        return;

    insertProcQ(&ready_queue, process);
}

pcb_t *dequeue_ready(pcb_t *p)
{
    if (p == NULL)
        return NULL;

    return outProcQ(&ready_queue, p);
}

pcb_t *create_process()
{
    pcb_t *new_process;

    if ((new_process = allocPcb()) == NULL)
        return NULL;

    increment_process_count();
    enqueue_ready(new_process);
    new_process->p_pid = get_new_pid();
    return new_process;
}

int kill_process(pcb_t *p)
{
    if (p == NULL)
        return 1;

    if (p->p_parent != NULL && outChild(p) != p)
        return 2;

    decrement_process_count();

    if (!list_empty(&p->p_list))
    {
        if (p->p_semAdd != NULL && outBlocked(p) == p)
        {
            soft_block_count--;
            p->p_semAdd = NULL;
        }
        else
        {
            outProcQ(&ready_queue, p);
        }
    }

    freePcb(p);
    return 0;
}

int kill_children(pcb_t *p)
{
    pcb_t *child;

    while ((child = removeChild(p)) != NULL)
    {
        kill_children(child);
    }

    return kill_process(p);
}

void schedule(pcb_t *new, int action)
{
    if (new != NULL && action == SCH_ENQUEUE)
        enqueue_ready(new);

    if (new != NULL && action == SCH_PRESERVE)
    {
        current_active_process = new;
    }
    else if (!list_empty(&ready_queue))
    {
        current_active_process = removeProcQ(&ready_queue);
    }
    else
    {
        if (process_count == 0)
        {
            halt("Halting from scheduler: Job's done! --");
        }
        else if (process_count > 0 && soft_block_count > 0)
        {
            reset_interval_timer();

            // interrupts on
            int status = getSTATUS();
            status |= IEPBITON;
            setSTATUS(status);

            wait("Waiting for i/o...\n");
        }
        else if (process_count > 0 && soft_block_count == 0)
        {
            panic("Houston, we have a deadlock!\n");
        }
    }

    PRINT_DEBUG("Scheduler status: process_count: %d, soft_block_count: %d\n", process_count, soft_block_count);

    if (current_active_process != NULL)
    {
        current_active_process->p_s.status |= TEBITON;
        PRINT_DEBUG("Process %d scheduled\n", current_active_process->p_pid);
        reset_plt();
        store_tod(&start_time);
        load_state(&(current_active_process->p_s));
    }
}

pcb_t *get_current_process()
{
    return current_active_process;
}

void decrement_process_count()
{
    process_count--;
}

void increment_process_count()
{
    process_count++;
}

void increment_softblocked_count()
{
    soft_block_count++;
}

void decrement_softblocked_count()
{
    soft_block_count--;
}

inline cpu_t get_start_time()
{
    return start_time;
}