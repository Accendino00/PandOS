#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <umps/libumps.h>

// For better readability and maintainability we define the 
// following constants and functions that manage the timers.

#define TIMESLICE 5000
#define ITINTERVAL 100000

HIDDEN inline void resetPLT() {
    setTIMER(TIMESLICE);
}

HIDDEN inline void resetIntervalTimer() {
    LDIT(ITINTERVAL);
}

#endif