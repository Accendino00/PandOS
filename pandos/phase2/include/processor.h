#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <umps/libumps.h>


// For better readability and maintainability we define the 
// following constants and functions that manage the timers.

/*
    We have the timer amount for PLT and the interval timer.
    To this, we add a small "extra time", which helps make it 
    more correct, since a bit of time is lost between setting 
    the timer and actually getting back to the process. 
*/
#define EXTRATIME 100
#define TIMESLICE (5000 + EXTRATIME)
#define ITINTERVAL (100000 + EXTRATIME)

static inline void resetPLT() {
    setTIMER(TRANSLATE_TIME(TIMESLICE));
}

static inline void resetIntervalTimer() {
    LDIT(ITINTERVAL);
}

#endif