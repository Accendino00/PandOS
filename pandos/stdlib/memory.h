#ifndef MEMORY_H
#define MEMORY_H

/**
 * @brief Copies size bytes from src to dest.
 * 
 * @param dest Destination
 * @param src  Source
 * @param size Size of copy
 */
void memcpy(void *dest, void *src, unsigned int size);

/**
 * @brief Sets the first len bytes of the block of memory pointed 
 * by s to the specified value c.
 * 
 * @param s         Start of memory block
 * @param c         Value to be set (interpreted as an unsigned char)
 * @param len       Number of bytes to be set to the value
 * @return void*    s
 */
extern void *memset(void *s, int c, unsigned int len);

#endif