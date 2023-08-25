#include <syscall.h>
#include <pandos_const.h>
#include <scheduler.h>
#include <print.h>
#include <memory.h>
#include <processor.h>
#include <semaphore.h>
#include <ns.h>
#include <pcb.h>
#include <interrupt.h>

extern scheduler_control_t passup_or_terminate(int cause);

const char* syscall_codes[] = {
    "CREATEPROCESS",
    "TERMPROCESS",
    "PASSEREN",
    "VERHOGEN",
    "DOIO",
    "GETTIME",
    "CLOCKWAIT",
    "GETSUPPORTPTR",
    "GETPROCESSID",
    "GETCHILDREN"};

scheduler_control_t syscall_handler()
{
    if (!is_kernel_mode())
    {
        PRINT_DEBUG("Syscall called in user mode\n");
        return passup_or_terminate(GENERALEXCEPT);
    }

    int id = (int)get_current_process()->p_s.reg_a0;

    PRINT_DEBUG("Syscall %s called by process %d.\n",syscall_codes[id], get_current_process()->p_pid);

    switch (id)
    {
    case CREATEPROCESS:
        return sys_create_process();
    case TERMPROCESS:
        return sys_terminate_process();
    case PASSEREN:
        return sys_passeren();
    case VERHOGEN:
        return sys_verhogen();
    case DOIO:
        sys_doio();
        break;
    case GETTIME:
        return sys_get_cpu_time();
        break;
    case CLOCKWAIT:
        return sys_wait_clock();
    case GETSUPPORTPTR:
        return sys_get_support_data();
    case GETPROCESSID:
        sys_get_process_id();
        break;
    case GETCHILDREN:
        sys_get_children();
        break;
    default:
        return passup_or_terminate(GENERALEXCEPT);
    }
}

scheduler_control_t sys_create_process()
{
    pcb_t *new_p;
    if ((new_p = create_process()) == NULL)
    {
        panic("Error while creating process\n");
    }

    insertChild(get_current_process(), new_p);

    state_t *state = FIRST_ARG(state_t *);
    support_t *support = SECOND_ARG(support_t *);
    nsd_t *namespace = THIRD_ARG(nsd_t *);

    if (state == NULL)
    {
        panic("Error while creating process: state is null\n");
    }

    // Initialization from arguments

    memcpy(&(new_p->p_s), state, sizeof(state_t));
    new_p->p_supportStruct = support;

    // #TODO - Check if namespace implementation works
    if (namespace == NULL)
    {
        // If the namespace is null, we are in the same namespace as the parent
        memcpy(new_p->namespaces, get_current_process()->namespaces, sizeof(nsd_t *) * NS_TYPE_MAX);
    }
    else
    {
        // If the namespace is not null, we are in the namespace passed as argument
        addNamespace(new_p, namespace);
    }

    RETURN_SYS(new_p->p_pid, int);

    return (scheduler_control_t){get_current_process(), SCH_PRESERVE};
}

scheduler_control_t sys_terminate_process()
{
    pcb_t *p_to_be_terminated;
    int pid = FIRST_ARG(int);

    if (pid == 0)
    {
        p_to_be_terminated = get_current_process();
    }
    else
    {
        // To-do: find process with pid
        // search within all pcbs if there is a process with field p_pid == pid
        // if not, return error

        int j = 0;
        for (j = 0; j < MAXPROC; j++)
        {
            if (get_pcb_table()[j].p_pid == pid)
            {
                p_to_be_terminated = &(get_pcb_table()[j]);
                break;
            }
        }

        if (j == MAXPROC)
        {
            // no process with pid found
            panic("Error while terminating process: no process with pid %d found\n", pid);
        }
    }

    kill_children(p_to_be_terminated);

    return pid == 0 ? (scheduler_control_t){NULL, SCH_BLOCK} : (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
}

scheduler_control_t sys_verhogen()
{
    pcb_t *p = V(FIRST_ARG(int *));
    return p == NULL ? (scheduler_control_t){NULL, SCH_BLOCK} : (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
}

scheduler_control_t sys_passeren()
{
    return P(FIRST_ARG(int *), get_current_process());
}

scheduler_control_t sys_wait_clock()
{
    return P(get_timer_semaphore(), get_current_process());
}

scheduler_control_t sys_get_cpu_time()
{
    RETURN_SYS(get_current_process()->p_time, cpu_t);

    return (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
}

void sys_doio()
{
}

scheduler_control_t sys_get_support_data()
{
    RETURN_SYS(get_current_process()->p_supportStruct, support_t *);

    return (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
}

void sys_get_process_id()
{
}

void sys_get_children()
{
}

int is_kernel_mode()
{
    /*
        We have to get the KUp, since we push the old KUc in the stack
        when we enter the exeptions. If KUp is 0, we are in kernel mode.

        The KUp is the 4rd bit in the status register.
    */
    return !((get_current_process()->p_s.status & KUPBITON) == KUPBITON);
}