#include "memory.h"

void memcpy(void *dest, void *src, unsigned int size)
{
    char *s = (char *)src;
    char *d = (char *)dest;
    for (unsigned int i = 0; i < size; ++i)
        d[i] = s[i];
}