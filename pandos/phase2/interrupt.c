#include "interrupt.h"
#include "print.h"
#include "processor.h"
#include "semaphore.h"

#include <umps/cp0.h>
#include <umps/arch.h>

static volatile int timer_semaphore = 0;

int *get_timer_semaphore()
{
    return &timer_semaphore;
}

scheduler_control_t interrupt_handler(int cause)
{
    PRINT_DEBUG("Interrupt handler --> cause: %b\n", cause);

    // interrupt 0: inter process communication
    if (cause & CAUSE_IP(0))
    {
        PRINT_DEBUG("Inter process communication interrupt\n");
        return (scheduler_control_t){get_current_process(), SCH_PRESERVE}; // preserve
    }

    // interrupt 1: processor local timer
    else if (cause & CAUSE_IP(1))
    {
        PRINT_DEBUG("Processor local timer interrupt\n");
        reset_plt(); // acknowledge the interrupt
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE}; // reschedule
    }

    // interrupt 2: interval timer
    else if (cause & CAUSE_IP(2))
    {
        PRINT_DEBUG("Interval timer interrupt\n");
        reset_interval_timer(); // acknowledge the interrupt
        while (timer_semaphore != 1)
        {
            V(&timer_semaphore);
        }
        return (scheduler_control_t){get_current_process(), SCH_PRESERVE}; // reschedule
    }

    // interrupt 3: disk
    else if (cause & CAUSE_IP(3))
    {
        PRINT_DEBUG("Disk interrupt\n");
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE}; // reschedule
    }

    // interrupt 4: tape
    else if (cause & CAUSE_IP(4))
    {
        PRINT_DEBUG("Tape interrupt\n");
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE}; // reschedule
    }

    // interrupt 5: network
    else if (cause & CAUSE_IP(5))
    {
        PRINT_DEBUG("Network interrupt\n");
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE}; // reschedule
    }

    // interrupt 6: printer
    else if (cause & CAUSE_IP(6))
    {
        PRINT_DEBUG("Printer interrupt\n");
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE}; // reschedule
    }

    // interrupt 7: terminal
    else if (cause & CAUSE_IP(7))
    {
        PRINT_DEBUG("Terminal interrupt\n");
        return (scheduler_control_t){get_current_process(), SCH_ENQUEUE}; // reschedule
    }

    // error
    PRINT_DEBUG("Error: interrupt cause not recognized\n");

    return (scheduler_control_t){get_current_process(), SCH_ENQUEUE};
}