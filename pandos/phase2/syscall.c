#include <syscall.h>
#include <pandos_const.h>
#include <scheduler.h>
#include <print.h>
#include <memory.h>

#include <pcb.h>

extern scheduler_control_t passup_or_terminate(int cause);

scheduler_control_t syscall_handler()
{
    int id = (int)get_current_process()->p_s.reg_a0;

    PRINT_DEBUG("Syscall called by process %d with id %d\n", get_current_process()->p_pid, id);

    switch (id)
    {
    case CREATEPROCESS:
        return sys_create_process();
    case TERMPROCESS:
        sys_terminate_process();
        break;
    case PASSEREN:
        sys_passeren();
        break;
    case VERHOGEN:
        sys_verhogen();
        break;
    case DOIO:
        sys_doio();
        break;
    case GETTIME:
        return sys_get_cpu_time();
        break;
    case CLOCKWAIT:
        sys_wait_clock();
        break;
    case GETSUPPORTPTR:
        sys_get_support_data();
        break;
    case GETPROCESSID:
        sys_get_process_id();
        break;
    case GETCHILDREN:
        sys_get_children();
        break;
    default:
        return passup_or_terminate(GENERALEXCEPT);
    }

    return (scheduler_control_t) {NULL, SCH_DO_NOTHING};
}

scheduler_control_t sys_create_process() {
    pcb_t* new_p;
    if((new_p = create_process()) == NULL) {
        PRINT_DEBUG("Error while creating process\n");
        return;
    }

    insertChild(get_current_process(), new_p);

    state_t* state = FIRST_ARG(state_t*);
    support_t *support = SECOND_ARG(support_t *);
    nsd_t *namespace = THIRD_ARG(nsd_t *);

    memcpy(&(new_p->p_s), state, sizeof(state_t));
    new_p->p_supportStruct = support;

    //TO-DO: add namespace handling

    RETURN_SYS(new_p->p_pid, int);

    return (scheduler_control_t) {new_p, SCH_MAKE_ACTIVE};
}

void sys_terminate_process() {
    pcb_t* p_to_be_terminated;
    int pid = FIRST_ARG(int);

    if(pid == 0) {
        p_to_be_terminated = get_current_process();
    } else {
        // To-do: find process with pid
    }

    kill_children(p_to_be_terminated);

    return;
}

void sys_verhogen() {

}

void sys_passeren() {

}

void sys_wait_clock() {

}

scheduler_control_t sys_get_cpu_time() {
    RETURN_SYS(get_current_process()->p_time, cpu_t);

    return (scheduler_control_t) {get_current_process(), SCH_DO_NOTHING};
}

void sys_doio() {

}

void sys_get_support_data() {
    RETURN_SYS(get_current_process()->p_supportStruct, support_t*);
}

void sys_get_process_id() {

}

void sys_get_children() {

}

