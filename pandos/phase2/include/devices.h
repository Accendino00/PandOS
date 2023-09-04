#ifndef DEVICES_H
#define DEVICES_H

typedef unsigned int semaphore_t;

/**
 * @brief Initializes the semaphores of the devices.
 * 
 * @note It sets the value of all the semaphores to 0. 
 */
extern void initDevSems();

/**
 * @brief Returns a pointer to the semaphore of the device.
 * 
 * @param index The index of the device.
 * 
 * @return semaphore_t* The pointer to the semaphore of the device.
 */
extern semaphore_t *getDevSem(unsigned int index);

/**
 * @brief Returns a pointer to the semaphore of the pseudo clock.
 * 
 * @return semaphore_t* The pointer to the semaphore of the pseudo clock.
 */
extern semaphore_t *getPseudoClockSem();

#endif