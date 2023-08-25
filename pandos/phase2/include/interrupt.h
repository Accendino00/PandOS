#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "scheduler.h"

int* get_timer_semaphore();

extern scheduler_control_t interrupt_handler(int cause);

#endif