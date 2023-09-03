#ifndef EXC_H
#define EXC_H

#include <umps/types.h>

void exceptionHandler();

extern void passUpOrDie(int excCode);

extern void syscallHandler(state_t*);

#endif