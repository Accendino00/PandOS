#ifndef DEVICES_H
#define DEVICES_H

typedef unsigned int semaphore_t;

extern void initDevSems();
extern semaphore_t *getDevSem(unsigned int index);
extern semaphore_t *getPseudoClockSem();

#endif