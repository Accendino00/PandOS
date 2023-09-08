#include "memory.h"

void memcpy(void *dest, void *src, unsigned int size)
{
    char *s = (char *)src;
    char *d = (char *)dest;
    for (unsigned int i = 0; i < size; ++i)
        d[i] = s[i];
}

void *memset(void *s, int c, unsigned int len)
{
    unsigned char *p = s;
    while (len--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}